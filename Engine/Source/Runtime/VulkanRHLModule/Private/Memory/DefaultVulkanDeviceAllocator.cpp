#include "../../Public/Memory/DefaultVulkanDeviceAllocator.h"
#include "Memory/VulkanMemoryCommon.h"

DefaultVulkanDeviceAllocator::DefaultVulkanDeviceAllocator(vk::raii::Device& device, const VulkanMemoryHeap& vulkanMemoryHeap,
    uint32_t memoryTypeIndex)
: m_device(device), m_memoryTypeIndex(memoryTypeIndex), m_vulkanMemoryHeap(vulkanMemoryHeap) {
    InitializeMemoryPool();
}

VulkanDeviceMemory DefaultVulkanDeviceAllocator::AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment) {
    size_t allocationTierIndex = 0;
    VulkanDeviceMemory allocatedMemory;
    if (!m_vulkanMemoryHeap.TryFindAllocationTier(memorySize, allocationTierIndex))
        return allocatedMemory;

    for (VulkanMemoryPage& tierMemoryPage: m_memoryPool[allocationTierIndex]) {
        if (tierMemoryPage.TryAllocateFromRange(memorySize, alignment, allocatedMemory))
            return allocatedMemory;
    }

    VulkanAllocationTier allocationTier = m_vulkanMemoryHeap.GetTierByIndex(allocationTierIndex);
    VulkanDeviceMemory allocatedPageMemory = Photon::VulkanMemoryCommon::AllocateDeviceMemory(m_device, allocationTier.m_pageSize,
        m_memoryTypeIndex);

    if (!allocatedPageMemory.IsValid())
        return allocatedMemory;

    VulkanMemoryPage& allocatedPage = m_memoryPool[allocationTierIndex].emplace_back(std::move(allocatedPageMemory));
    VulkanDeviceMemory memoryFromPage;
    allocatedPage.TryAllocateFromRange(memorySize, alignment, memoryFromPage);
    return memoryFromPage;
}

void DefaultVulkanDeviceAllocator::FreeMemory(VulkanDeviceMemory& memory) {
    if (memory.GetMemoryTypeIndex() != m_memoryTypeIndex)
        return;

    size_t allocationTierIndex = 0;
    if (!m_vulkanMemoryHeap.TryFindAllocationTier(memory.GetAllocationSize(), allocationTierIndex))
        return;

    auto pageToReturnMemory = std::ranges::find_if(m_memoryPool[allocationTierIndex],
        [&memory](VulkanMemoryPage& memoryPage) {
            return memoryPage.GetPageMemory().GetHandle() == memory.GetHandle();
    });

    if (pageToReturnMemory == m_memoryPool[allocationTierIndex].end())
        return;

    pageToReturnMemory->ReturnMemory(memory);
}

void DefaultVulkanDeviceAllocator::FreeAllMemory(bool deallocate) {
    for (std::vector<VulkanMemoryPage>& memoryPages : m_memoryPool) {
        for (VulkanMemoryPage& page: memoryPages) {
            if (deallocate)
                Photon::VulkanMemoryCommon::FreeDeviceMemory(m_device, page.GetPageMemory());
            else
                page.InvalidateMemory();
        }

        memoryPages.clear();
    }
}

void DefaultVulkanDeviceAllocator::InitializeMemoryPool() {
    m_memoryPool.resize(m_vulkanMemoryHeap.GetTiersSize());
}