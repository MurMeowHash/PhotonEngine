#include "../../Public/Memory/DefaultVulkanSeparateDeviceAllocator.h"
#include "Device/VulkanDevice.h"
#include "Memory/VulkanMemoryCommon.h"
#include <limits>

DefaultVulkanSeparateDeviceAllocator::DefaultVulkanSeparateDeviceAllocator(const DefaultVulkanSeparateDeviceAllocatorCreateInfo &createInfo)
: m_vulkanDevice(createInfo.m_vulkanDevice), m_memoryTypeIndex(createInfo.m_memoryTypeIndex),
m_memoryPool({createInfo.m_vulkanDevice, createInfo.m_memoryTypeIndex}) { }

VulkanDeviceMemory DefaultVulkanSeparateDeviceAllocator::AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment) {
    return m_memoryPool.Allocate(memorySize, alignment, nullptr);
}

vk::DeviceSize DefaultVulkanSeparateDeviceAllocator::GetMaxAllocationSize() const {
    return std::numeric_limits<vk::DeviceSize>::max();
}

VulkanDeviceMemory DefaultVulkanSeparateDeviceAllocator::AllocateImageMemoryVkDedicated(vk::Image image) {
    VkMemoryDedicatedAllocateInfo dedicatedAllocateInfo{};
    dedicatedAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
    dedicatedAllocateInfo.image = image;
    vk::MemoryRequirements imageMemoryRequirements{};
    m_vulkanDevice->GetRawHandle().getImageMemoryRequirements(image, &imageMemoryRequirements);
    return m_memoryPool.Allocate(imageMemoryRequirements.size, imageMemoryRequirements.alignment, &dedicatedAllocateInfo);
}

VulkanDeviceMemory DefaultVulkanSeparateDeviceAllocator::AllocateBufferMemoryVkDedicated(vk::Buffer buffer) {
    VkMemoryDedicatedAllocateInfo dedicatedAllocateInfo{};
    dedicatedAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
    dedicatedAllocateInfo.buffer = buffer;
    vk::MemoryRequirements bufferMemoryRequirements{};
    m_vulkanDevice->GetRawHandle().getBufferMemoryRequirements(buffer, &bufferMemoryRequirements);
    return m_memoryPool.Allocate(bufferMemoryRequirements.size, bufferMemoryRequirements.alignment, &dedicatedAllocateInfo);
}

void DefaultVulkanSeparateDeviceAllocator::FreeMemory(VulkanDeviceMemory &memory) {
    m_memoryPool.Free(memory);
}

void DefaultVulkanSeparateDeviceAllocator::FreeAllMemory() {
    m_memoryPool.FreeAll();
}