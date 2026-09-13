#include "../../Public/Memory/VulkanDeviceMemoryProvider.h"
#include "CoreUtils.h"
#include "Memory/DefaultVulkanDeviceAllocator.h"
#include "LinearAllocator.h"

const std::string VulkanDeviceMemoryProvider::MEMORY_ALLOCATION_TILING_ATTRIBUTE = "MEMORY_ALLOCATION_TILING_ATTRIBUTE";

VulkanDeviceMemoryProvider::VulkanDeviceMemoryProvider(vk::raii::Device &device, vk::raii::PhysicalDevice &physicalDevice,
    const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition)
: m_device(device), m_physicalDevice(physicalDevice) {
    m_allocator = new LinearAllocator(CHUNK_SIZE_FOR_DEVICE_ALLOCATORS);
    InitializeMemoryProperties();
    InitializeMemoryHeaps(vulkanAllocationPresetDefinition);
}

VulkanDeviceMemoryProvider::~VulkanDeviceMemoryProvider() {
    VulkanDeviceMemoryProvider::FreeAllMemory();
    m_allocator->FreeMemory();
    delete m_allocator;
}

VulkanDeviceMemory VulkanDeviceMemoryProvider::AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment,
                                                              uint32_t memoryTypeIndex, VulkanResourceTilingType resourceTiling) {
    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(resourceTiling, memoryTypeIndex);
    VulkanDeviceMemory allocatedDeviceMemory = allocatorBundle.GetDeviceAllocator()->AllocateMemory(memorySize, alignment);
    allocatedDeviceMemory.GetMemoryAttributes().OverrideAttribute(MEMORY_ALLOCATION_TILING_ATTRIBUTE, VulkanResourceTilingType::Linear);
    return allocatedDeviceMemory;
}

VulkanDeviceMemory VulkanDeviceMemoryProvider::AllocateBufferMemory(const vk::raii::Buffer &buffer, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
    DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) {
    vk::BufferMemoryRequirementsInfo2 bufferMemoryRequirementsInfo;
    bufferMemoryRequirementsInfo.buffer = *buffer;
    vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements> memoryRequirementsChain =
        m_device.getBufferMemoryRequirements2<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements>(bufferMemoryRequirementsInfo);

    vk::MemoryRequirements2 memoryRequirements2 = memoryRequirementsChain.get<vk::MemoryRequirements2>();
    vk::MemoryDedicatedRequirements memoryDedicatedRequirements = memoryRequirementsChain.get<vk::MemoryDedicatedRequirements>();

    VulkanDeviceMemory allocatedDeviceMemory;
    uint32_t memoryTypeIndex = 0;
    if (!TryFindMemoryTypeIndex(memoryRequirements2.memoryRequirements.memoryTypeBits, requestedMemoryPropertyFlags, memoryTypeIndex))
        return allocatedDeviceMemory;

    bool allocateDedicatedMemory = ShouldAllocateDedicatedMemory(memoryDedicatedRequirements, dedicatedMemoryRequirementsFlags);
    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(VulkanResourceTilingType::Linear, memoryTypeIndex);

    if (allocateDedicatedMemory)
        allocatedDeviceMemory = allocatorBundle.GetDedicatedDeviceAllocator()->AllocateBufferMemory(buffer);
    else
        allocatedDeviceMemory = allocatorBundle.GetDeviceAllocator()->AllocateMemory(memoryRequirements2.memoryRequirements.size,
            memoryRequirements2.memoryRequirements.alignment);

    allocatedDeviceMemory.GetMemoryAttributes().OverrideAttribute(MEMORY_ALLOCATION_TILING_ATTRIBUTE, VulkanResourceTilingType::Linear);

    return allocatedDeviceMemory;
}

VulkanDeviceMemory VulkanDeviceMemoryProvider::AllocateImageMemory(const vk::raii::Image &image, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
    DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags, VulkanResourceTilingType resourceTiling) {
    vk::ImageMemoryRequirementsInfo2 imageMemoryRequirementsInfo;
    imageMemoryRequirementsInfo.image = *image;

    vk::StructureChain<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements> memoryRequirementsChain =
        m_device.getImageMemoryRequirements2<vk::MemoryRequirements2, vk::MemoryDedicatedRequirements>(imageMemoryRequirementsInfo);

    vk::MemoryRequirements2 memoryRequirements2 = memoryRequirementsChain.get<vk::MemoryRequirements2>();
    vk::MemoryDedicatedRequirements memoryDedicatedRequirements = memoryRequirementsChain.get<vk::MemoryDedicatedRequirements>();

    VulkanDeviceMemory allocatedDeviceMemory;
    uint32_t memoryTypeIndex = 0;
    if (!TryFindMemoryTypeIndex(memoryRequirements2.memoryRequirements.memoryTypeBits, requestedMemoryPropertyFlags, memoryTypeIndex))
        return allocatedDeviceMemory;

    bool allocateDedicatedMemory = ShouldAllocateDedicatedMemory(memoryDedicatedRequirements, dedicatedMemoryRequirementsFlags);
    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(resourceTiling, memoryTypeIndex);

    if (allocateDedicatedMemory)
        allocatedDeviceMemory = allocatorBundle.GetDedicatedDeviceAllocator()->AllocateImageMemory(image);
    else
        allocatedDeviceMemory = allocatorBundle.GetDeviceAllocator()->AllocateMemory(memoryRequirements2.memoryRequirements.size,
            memoryRequirements2.memoryRequirements.alignment);

    allocatedDeviceMemory.GetMemoryAttributes().OverrideAttribute(MEMORY_ALLOCATION_TILING_ATTRIBUTE, resourceTiling);

    return allocatedDeviceMemory;
}

bool VulkanDeviceMemoryProvider::TryFindMemoryTypeIndex(uint32_t memoryTypeRequirementsBits,
    vk::MemoryPropertyFlags requestedMemoryPropertyFlags,uint32_t &memoryTypeIndex) {
    for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++) {
        if (memoryTypeRequirementsBits & 1 << i
            && (m_memoryProperties.memoryTypes[i].propertyFlags & requestedMemoryPropertyFlags) == requestedMemoryPropertyFlags) {
            memoryTypeIndex = i;
            return true;
        }
    }

    return false;
}

void VulkanDeviceMemoryProvider::FreeMemory(VulkanDeviceMemory &deviceMemory) {
    VulkanResourceTilingType resourceTiling = VulkanResourceTilingType::None;

    if (!deviceMemory.GetMemoryAttributes().TryRetrieveAttribute(MEMORY_ALLOCATION_TILING_ATTRIBUTE, resourceTiling)
        || resourceTiling == VulkanResourceTilingType::None)
        return;

    VulkanDeviceAllocatorBundle& allocatorBundle = GetAllocatorBundle(resourceTiling, deviceMemory.GetMemoryTypeIndex());
    if (deviceMemory.IsDedicated())
        allocatorBundle.GetDedicatedDeviceAllocator()->FreeMemory(deviceMemory);
    else
        allocatorBundle.GetDeviceAllocator()->FreeMemory(deviceMemory);
}

void VulkanDeviceMemoryProvider::FreeAllMemory() {
    for (std::pair<const VulkanResourceTilingType, VulkanAllocatorsCollection>& vulkanAllocatorCollection : m_vulkanAllocators) {
        for (VulkanDeviceAllocatorBundle& allocatorBundle: vulkanAllocatorCollection.second.m_allocatorBundles) {
            IVulkanDeviceAllocator* deviceAllocator;
            if (allocatorBundle.TryGetDeviceAllocator(deviceAllocator))
                deviceAllocator->FreeAllMemory(true);

            IVulkanDedicatedDeviceAllocator *dedicatedDeviceAllocator;
            if (allocatorBundle.TryGetDedicatedDeviceAllocator(dedicatedDeviceAllocator))
                dedicatedDeviceAllocator->FreeAllMemory();
        }
    }
}

VulkanDeviceAllocatorBundle& VulkanDeviceMemoryProvider::GetAllocatorBundle(VulkanResourceTilingType resourceTiling, uint32_t memoryTypeIndex) {
    auto allocatorsCollectionIterator = m_vulkanAllocators.find(resourceTiling);
    if (allocatorsCollectionIterator == m_vulkanAllocators.end()) {
        allocatorsCollectionIterator = InitializeAllocatorsCollection(resourceTiling);
    }

    assert(memoryTypeIndex < allocatorsCollectionIterator->second.m_allocatorBundles.size());
    return allocatorsCollectionIterator->second.m_allocatorBundles[memoryTypeIndex];
}

void VulkanDeviceMemoryProvider::InitializeMemoryProperties() {
    m_memoryProperties = m_physicalDevice.getMemoryProperties();
}

void VulkanDeviceMemoryProvider::InitializeMemoryHeaps(const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition) {
    for (vk::MemoryHeap memoryHeap: m_memoryProperties.memoryHeaps) {
        m_vulkanMemoryHeaps.emplace_back(memoryHeap, vulkanAllocationPresetDefinition);
    }
}

std::unordered_map<VulkanResourceTilingType, VulkanAllocatorsCollection>::iterator
VulkanDeviceMemoryProvider::InitializeAllocatorsCollection(VulkanResourceTilingType resourceTiling) {
    VulkanAllocatorsCollection allocatorsCollection;
    allocatorsCollection.m_allocatorBundles.reserve(m_memoryProperties.memoryTypeCount);
    for (uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; i++) {
        allocatorsCollection.m_allocatorBundles.emplace_back(m_device, i, m_vulkanMemoryHeaps[m_memoryProperties.memoryTypes[i].heapIndex], m_allocator);
    }

    auto allocatorsCollectionIterator = m_vulkanAllocators.emplace(resourceTiling, allocatorsCollection);
    return allocatorsCollectionIterator.first;
}

bool VulkanDeviceMemoryProvider::ShouldAllocateDedicatedMemory(const vk::MemoryDedicatedRequirements &dedicatedMemoryRequirements,
    DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) const {
    return dedicatedMemoryRequirements.requiresDedicatedAllocation
    || (dedicatedMemoryRequirements.prefersDedicatedAllocation
    && Photon::Core::Flags::IsFlagSet(dedicatedMemoryRequirementsFlags, DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred))
    || Photon::Core::Flags::IsFlagSet(dedicatedMemoryRequirementsFlags, DedicatedMemoryRequirementsFlags::ForceDedicated);
}