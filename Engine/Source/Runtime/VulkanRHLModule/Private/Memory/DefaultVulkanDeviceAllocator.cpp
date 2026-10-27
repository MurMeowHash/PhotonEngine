#include "../../Public/Memory/DefaultVulkanDeviceAllocator.h"
#include "Memory/VulkanMemoryCommon.h"

DefaultVulkanDeviceAllocator::DefaultVulkanDeviceAllocator(const DefaultVulkanDeviceAllocatorCreateInfo &createInfo)
: m_vulkanDevice(createInfo.m_vulkanDevice), m_memoryTypeIndex(createInfo.m_memoryTypeIndex), m_vulkanMemoryHeap(createInfo.m_vulkanMemoryHeap) {
    InitializeMemoryPool(createInfo.m_releaseEmptyPages);
}

VulkanDeviceMemory DefaultVulkanDeviceAllocator::AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment) {
    size_t allocationTierIndex = 0;
    VulkanDeviceMemory allocatedMemory{};
    if (!m_vulkanMemoryHeap.TryFindAllocationTier(memorySize, allocationTierIndex))
        return allocatedMemory;

    return m_memoryPool[allocationTierIndex].Allocate(memorySize, alignment);
}

vk::DeviceSize DefaultVulkanDeviceAllocator::GetMaxAllocationSize() const {
    return m_vulkanMemoryHeap.GetMaxAllowedAllocation();
}

void DefaultVulkanDeviceAllocator::FreeMemory(VulkanDeviceMemory& memory) {
    if (memory.GetMemoryTypeIndex() != m_memoryTypeIndex)
        return;

    const uint32_t allocationTierIndex = memory.GetAllocationTier();
    if (allocationTierIndex >= m_memoryPool.size())
        return;

    m_memoryPool[allocationTierIndex].Free(memory);
}

void DefaultVulkanDeviceAllocator::FreeAllMemory() {
    for (VulkanDeviceMemoryPagesAllocator& memPageAllocator : m_memoryPool) {
        memPageAllocator.FreeAll();
    }
}

void DefaultVulkanDeviceAllocator::InitializeMemoryPool(bool releaseEmptyPages) {
    const uint32_t memTiersSize = m_vulkanMemoryHeap.GetTiersSize();
    m_memoryPool.reserve(memTiersSize);

    for (uint32_t tierIndex = 0; tierIndex < m_vulkanMemoryHeap.GetTiersSize(); tierIndex++) {
        VulkanMemoryPagesAllocatorCreateInfo createInfo{
            m_vulkanDevice,
            m_memoryTypeIndex,
            tierIndex,
            m_vulkanMemoryHeap.GetTierByIndex(tierIndex).m_pageSize,
            releaseEmptyPages
        };

        m_memoryPool.emplace_back(createInfo);
    }
}