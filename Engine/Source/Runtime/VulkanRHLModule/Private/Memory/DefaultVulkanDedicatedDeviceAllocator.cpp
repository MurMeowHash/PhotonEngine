#include "../../Public/Memory/DefaultVulkanDedicatedDeviceAllocator.h"
#include "Memory/VulkanMemoryCommon.h"

DefaultVulkanDedicatedDeviceAllocator::DefaultVulkanDedicatedDeviceAllocator(vk::raii::Device& device, uint32_t memoryTypeIndex)
: m_device(device), m_memoryTypeIndex(memoryTypeIndex) { }

VulkanDeviceMemory DefaultVulkanDedicatedDeviceAllocator::AllocateImageMemory(const vk::raii::Image &image) {
    VulkanDeviceMemory allocatedMemory = Photon::VulkanMemoryCommon::AllocateDedicatedImageMemory(m_device, image, m_memoryTypeIndex);
    return TryAddPageForMemory(std::move(allocatedMemory));
}

VulkanDeviceMemory DefaultVulkanDedicatedDeviceAllocator::AllocateBufferMemory(const vk::raii::Buffer &buffer) {
    VulkanDeviceMemory allocatedMemory = Photon::VulkanMemoryCommon::AllocateDedicatedBufferMemory(m_device, buffer, m_memoryTypeIndex);
    return TryAddPageForMemory(std::move(allocatedMemory));
}

void DefaultVulkanDedicatedDeviceAllocator::FreeMemory(VulkanDeviceMemory &memory) {
    std::erase_if(m_memoryPool, [&memory](VulkanMemoryPage& memoryPage) {
        return memoryPage.GetPageMemory().GetHandle() == memory.GetHandle();
    });

    Photon::VulkanMemoryCommon::FreeDeviceMemory(m_device, memory);
}

void DefaultVulkanDedicatedDeviceAllocator::FreeAllMemory() {
    for (VulkanMemoryPage& memoryPage : m_memoryPool) {
        Photon::VulkanMemoryCommon::FreeDeviceMemory(m_device, memoryPage.GetPageMemory());
    }

    m_memoryPool.clear();
}

VulkanDeviceMemory DefaultVulkanDedicatedDeviceAllocator::TryAddPageForMemory(VulkanDeviceMemory&& memory) {
    if (!memory.IsValid())
        return memory;

    VulkanMemoryPage& dedicatedMemoryPage = m_memoryPool.emplace_back(std::move(memory));
    return dedicatedMemoryPage.GetPageMemory();
}