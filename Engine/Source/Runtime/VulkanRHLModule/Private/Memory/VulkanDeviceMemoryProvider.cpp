#include "../../Public/Memory/VulkanDeviceMemoryProvider.h"
#include "CoreUtils.h"
#include "LinearAllocator.h"
#include "Device/VulkanDevice.h"

VulkanDeviceMemoryProvider::VulkanDeviceMemoryProvider(const VulkanDeviceMemoryProviderCreateInfo &createInfo)
: m_vulkanDevice(createInfo.m_vulkanDevice), m_releaseEmptyPages(createInfo.m_releaseEmptyPages) {
    m_allocator = new LinearAllocator(CHUNK_SIZE_FOR_DEVICE_ALLOCATORS);
    m_memoryProperties = m_vulkanDevice->GetMemoryProperties();
    InitializeMemoryHeaps(createInfo.m_vulkanAllocationPresetDefinition);
}

VulkanDeviceMemoryProvider::~VulkanDeviceMemoryProvider() {
    VulkanDeviceMemoryProvider::FreeAllMemory();
    m_allocator->FreeMemory();
    delete m_allocator;
}

VulkanDeviceMemoryAllocation VulkanDeviceMemoryProvider::AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment,
    uint32_t memoryTypeIndex, VulkanResourceTilingType resourceTiling) {
    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(resourceTiling, memoryTypeIndex);
    return AllocateFromBundle(allocatorBundle, memorySize, alignment, resourceTiling);
}

VulkanDeviceMemoryAllocation VulkanDeviceMemoryProvider::AllocateBufferMemory(vk::Buffer buffer, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
    DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) {
    vk::BufferMemoryRequirementsInfo2 bufferMemoryRequirementsInfo{};
    bufferMemoryRequirementsInfo.buffer = buffer;
    vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements> memoryRequirementsChain =
        m_vulkanDevice->GetRawHandle().getBufferMemoryRequirements2<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements>(bufferMemoryRequirementsInfo);

    const vk::MemoryRequirements& memoryRequirements = memoryRequirementsChain.get<vk::MemoryRequirements2>().memoryRequirements;
    const vk::MemoryDedicatedRequirements& memoryDedicatedRequirements = memoryRequirementsChain.get<vk::MemoryDedicatedRequirements>();

    uint32_t memoryTypeIndex = 0;
    if (!TryFindMemoryTypeIndex(memoryRequirements.memoryTypeBits, requestedMemoryPropertyFlags, memoryTypeIndex))
        return VulkanDeviceMemoryAllocation{};

    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(VulkanResourceTilingType::Linear, memoryTypeIndex);
    if (ShouldAllocateDedicatedMemory(memoryDedicatedRequirements, dedicatedMemoryRequirementsFlags))
        return MakeAllocation(allocatorBundle.GetSeparateDeviceAllocator()->AllocateBufferMemoryVkDedicated(buffer),
            VulkanResourceTilingType::Linear, VulkanDeviceMemoryAllocatorType::Separate);

    return AllocateFromBundle(allocatorBundle, memoryRequirements.size, memoryRequirements.alignment, VulkanResourceTilingType::Linear);
}

VulkanDeviceMemoryAllocation VulkanDeviceMemoryProvider::AllocateImageMemory(vk::Image image, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
    DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags, VulkanResourceTilingType resourceTiling) {
    vk::ImageMemoryRequirementsInfo2 imageMemoryRequirementsInfo{};
    imageMemoryRequirementsInfo.image = image;
    vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements> memoryRequirementsChain =
        m_vulkanDevice->GetRawHandle().getImageMemoryRequirements2<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements>(imageMemoryRequirementsInfo);

    const vk::MemoryRequirements& memoryRequirements = memoryRequirementsChain.get<vk::MemoryRequirements2>().memoryRequirements;
    const vk::MemoryDedicatedRequirements& memoryDedicatedRequirements = memoryRequirementsChain.get<vk::MemoryDedicatedRequirements>();

    uint32_t memoryTypeIndex = 0;
    if (!TryFindMemoryTypeIndex(memoryRequirements.memoryTypeBits, requestedMemoryPropertyFlags, memoryTypeIndex))
        return VulkanDeviceMemoryAllocation{};

    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(resourceTiling, memoryTypeIndex);
    if (ShouldAllocateDedicatedMemory(memoryDedicatedRequirements, dedicatedMemoryRequirementsFlags))
        return MakeAllocation(allocatorBundle.GetSeparateDeviceAllocator()->AllocateImageMemoryVkDedicated(image),
            resourceTiling, VulkanDeviceMemoryAllocatorType::Separate);

    return AllocateFromBundle(allocatorBundle, memoryRequirements.size, memoryRequirements.alignment, resourceTiling);
}

bool VulkanDeviceMemoryProvider::TryFindMemoryTypeIndex(uint32_t memoryTypeRequirementsBits,
    vk::MemoryPropertyFlags requestedMemoryPropertyFlags, uint32_t &memoryTypeIndex) {
    const vk::MemoryPropertyFlags avoidedFlags = vk::MemoryPropertyFlagBits::eProtected
        | vk::MemoryPropertyFlagBits::eLazilyAllocated
        | vk::MemoryPropertyFlagBits::eDeviceCoherentAMD
        | vk::MemoryPropertyFlagBits::eDeviceUncachedAMD;

    uint32_t bestMemoryTypeIndex = UINT32_MAX;
    int bestExtraFlagsCount = INT_MAX;
    for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++) {
        if (!(memoryTypeRequirementsBits & (1u << i)))
            continue;

        const vk::MemoryPropertyFlags typeFlags = m_memoryProperties.memoryTypes[i].propertyFlags;
        if ((typeFlags & requestedMemoryPropertyFlags) != requestedMemoryPropertyFlags)
            continue;

        const vk::MemoryPropertyFlags extraFlags = typeFlags & ~requestedMemoryPropertyFlags;
        if (extraFlags & avoidedFlags)
            continue;

        const int extraFlagsCount = std::popcount(static_cast<VkMemoryPropertyFlags>(extraFlags));
        if (extraFlagsCount < bestExtraFlagsCount) {
            bestExtraFlagsCount = extraFlagsCount;
            bestMemoryTypeIndex = i;
        }
    }

    if (bestMemoryTypeIndex == UINT32_MAX)
        return false;

    memoryTypeIndex = bestMemoryTypeIndex;
    return true;
}

void VulkanDeviceMemoryProvider::FreeMemory(VulkanDeviceMemoryAllocation &allocation) {
    if (!allocation.IsValid() || allocation.m_tiling == VulkanResourceTilingType::None)
        return;

    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(allocation.m_tiling, allocation.m_memory.GetMemoryTypeIndex());
    switch (allocation.m_allocatorType) {
        case VulkanDeviceMemoryAllocatorType::Pooled:
            allocatorBundle.GetDeviceAllocator()->FreeMemory(allocation.m_memory);
            break;
        case VulkanDeviceMemoryAllocatorType::Separate:
            allocatorBundle.GetSeparateDeviceAllocator()->FreeMemory(allocation.m_memory);
            break;
        default:
            assert(false);
    }

    allocation = VulkanDeviceMemoryAllocation{};
}

void VulkanDeviceMemoryProvider::FreeAllMemory() {
    for (std::pair<const VulkanResourceTilingType, VulkanAllocatorsCollection>& vulkanAllocatorCollection : m_vulkanAllocators) {
        for (VulkanDeviceAllocatorBundle& allocatorBundle: vulkanAllocatorCollection.second.m_allocatorBundles) {
            IVulkanDeviceAllocator* deviceAllocator;
            if (allocatorBundle.TryGetDeviceAllocator(deviceAllocator))
                deviceAllocator->FreeAllMemory();

            IVulkanSeparateDeviceAllocator* separateDeviceAllocator;
            if (allocatorBundle.TryGetSeparateDeviceAllocator(separateDeviceAllocator))
                separateDeviceAllocator->FreeAllMemory();
        }
    }
}

VulkanDeviceMemoryAllocation VulkanDeviceMemoryProvider::AllocateFromBundle(VulkanDeviceAllocatorBundle &allocatorBundle,
    vk::DeviceSize memorySize, vk::DeviceSize alignment, VulkanResourceTilingType resourceTiling) {
    IVulkanDeviceAllocator* pooledAllocator = allocatorBundle.GetDeviceAllocator();
    if (memorySize <= pooledAllocator->GetMaxAllocationSize())
        return MakeAllocation(pooledAllocator->AllocateMemory(memorySize, alignment), resourceTiling, VulkanDeviceMemoryAllocatorType::Pooled);

    return MakeAllocation(allocatorBundle.GetSeparateDeviceAllocator()->AllocateMemory(memorySize, alignment),
        resourceTiling, VulkanDeviceMemoryAllocatorType::Separate);
}

VulkanDeviceMemoryAllocation VulkanDeviceMemoryProvider::MakeAllocation(const VulkanDeviceMemory &memory,
    VulkanResourceTilingType resourceTiling, VulkanDeviceMemoryAllocatorType allocatorType) {
    if (!memory.IsValid())
        return VulkanDeviceMemoryAllocation{};

    return VulkanDeviceMemoryAllocation{memory, resourceTiling, allocatorType};
}

VulkanDeviceAllocatorBundle& VulkanDeviceMemoryProvider::GetAllocatorBundle(VulkanResourceTilingType resourceTiling, uint32_t memoryTypeIndex) {
    auto allocatorsCollectionIterator = m_vulkanAllocators.find(resourceTiling);
    if (allocatorsCollectionIterator == m_vulkanAllocators.end()) {
        allocatorsCollectionIterator = InitializeAllocatorsCollection(resourceTiling);
    }

    assert(memoryTypeIndex < allocatorsCollectionIterator->second.m_allocatorBundles.size());
    return allocatorsCollectionIterator->second.m_allocatorBundles[memoryTypeIndex];
}

void VulkanDeviceMemoryProvider::InitializeMemoryHeaps(const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition) {
    m_vulkanMemoryHeaps.reserve(m_memoryProperties.memoryHeapCount);
    for (uint32_t i = 0; i < m_memoryProperties.memoryHeapCount; i++) {
        m_vulkanMemoryHeaps.emplace_back(m_memoryProperties.memoryHeaps[i], vulkanAllocationPresetDefinition);
    }
}

std::unordered_map<VulkanResourceTilingType, VulkanAllocatorsCollection>::iterator
VulkanDeviceMemoryProvider::InitializeAllocatorsCollection(VulkanResourceTilingType resourceTiling) {
    VulkanAllocatorsCollection allocatorsCollection;
    allocatorsCollection.m_allocatorBundles.reserve(m_memoryProperties.memoryTypeCount);
    for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++) {
        VulkanDeviceAllocatorBundleCreateInfo bundleCreateInfo{
            m_vulkanDevice,
            i,
            m_vulkanMemoryHeaps[m_memoryProperties.memoryTypes[i].heapIndex],
            m_allocator,
            m_releaseEmptyPages
        };

        allocatorsCollection.m_allocatorBundles.emplace_back(bundleCreateInfo);
    }

    auto allocatorsCollectionIterator = m_vulkanAllocators.emplace(resourceTiling, std::move(allocatorsCollection));
    return allocatorsCollectionIterator.first;
}

bool VulkanDeviceMemoryProvider::ShouldAllocateDedicatedMemory(const vk::MemoryDedicatedRequirements &dedicatedMemoryRequirements,
    DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) const {
    return dedicatedMemoryRequirements.requiresDedicatedAllocation
    || (dedicatedMemoryRequirements.prefersDedicatedAllocation
    && IS_FLAG_SET(dedicatedMemoryRequirementsFlags, DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred))
    || IS_FLAG_SET(dedicatedMemoryRequirementsFlags, DedicatedMemoryRequirementsFlags::ForceDedicated);
}