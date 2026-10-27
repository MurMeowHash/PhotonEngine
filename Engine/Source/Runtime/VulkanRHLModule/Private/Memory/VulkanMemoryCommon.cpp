#include "../../Public/Memory/VulkanMemoryCommon.h"
#include "Device/VulkanDevice.h"

namespace Photon::VulkanMemoryCommon {
    VulkanDeviceMemory AllocateDeviceMemory(VulkanDevice* vulkanDevice, vk::DeviceSize memSize, uint32_t memoryTypeIndex,
        VkMemoryDedicatedAllocateInfo* dedicatedAllocationInfo) {
        VkMemoryAllocateInfo memoryAllocateInfo;
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memSize;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
        memoryAllocateInfo.pNext = dedicatedAllocationInfo;
        VkDeviceMemory allocatedMemory;
        VkResult allocationResult = vkAllocateMemory(vulkanDevice->GetRawHandle(), &memoryAllocateInfo, nullptr, &allocatedMemory);
        if (allocationResult != VK_SUCCESS)
            return VulkanDeviceMemory();

        VulkanDeviceMemoryInitializeInfo initializeInfo{};
        initializeInfo.m_vulkanDevice = vulkanDevice;
        initializeInfo.m_handle = allocatedMemory;
        initializeInfo.m_memoryTypeIndex = memoryTypeIndex;
        initializeInfo.m_dedicated = dedicatedAllocationInfo != nullptr
            && (dedicatedAllocationInfo->image != nullptr || dedicatedAllocationInfo->buffer != nullptr);
        initializeInfo.m_memInfo.m_size = memSize;
        initializeInfo.m_memInfo.m_offset = 0;
        initializeInfo.m_memInfo.m_alignmentOffset = 0;
        initializeInfo.m_mappedRange = nullptr;
        return VulkanDeviceMemory(initializeInfo);
    }

    void FreeDeviceMemory(vk::Device device, VulkanDeviceMemory &memory) {
        if (!memory.IsValid())
            return;

        vkFreeMemory(device, memory.GetHandle(), nullptr);
        memory.InvalidateMemory();
    }

    VulkanDeviceMemory AllocateDedicatedImageMemory(VulkanDevice* vulkanDevice, const vk::raii::Image& image,
        uint32_t memoryTypeIndex) {
        VkMemoryDedicatedAllocateInfo dedicatedMemoryAllocateInfo;
        dedicatedMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
        dedicatedMemoryAllocateInfo.image = *image;
        vk::MemoryRequirements imageMemoryRequirements = image.getMemoryRequirements();
        return AllocateDeviceMemory(vulkanDevice, imageMemoryRequirements.size, memoryTypeIndex, &dedicatedMemoryAllocateInfo);
    }

    VulkanDeviceMemory AllocateDedicatedBufferMemory(VulkanDevice* vulkanDevice, vk::Buffer buffer,
        uint32_t memoryTypeIndex) {
        VkMemoryDedicatedAllocateInfo dedicatedMemoryAllocateInfo;
        dedicatedMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
        dedicatedMemoryAllocateInfo.buffer = buffer;
        vk::MemoryRequirements bufferMemoryRequirements;
        vulkanDevice->GetRawHandle().getBufferMemoryRequirements(buffer, &bufferMemoryRequirements);
        return AllocateDeviceMemory(vulkanDevice, bufferMemoryRequirements.size, memoryTypeIndex, &dedicatedMemoryAllocateInfo);
    }

    vk::Result BindImageMemory(const vk::raii::Device &device, const VulkanDeviceMemory &memory, const vk::raii::Image &image) {
        vk::BindImageMemoryInfo bindImageMemoryInfo;
        bindImageMemoryInfo.image = *image;
        bindImageMemoryInfo.memory = memory.GetHandle();
        bindImageMemoryInfo.memoryOffset = memory.GetOffset();
        return device.bindImageMemory2(bindImageMemoryInfo);
    }

    vk::Result BindBufferMemory(vk::Device device, const VulkanDeviceMemory &memory, vk::Buffer buffer) {
        vk::BindBufferMemoryInfo bindBufferMemoryInfo;
        bindBufferMemoryInfo.buffer = buffer;
        bindBufferMemoryInfo.memory = memory.GetHandle();
        bindBufferMemoryInfo.memoryOffset = memory.GetOffset();
        return device.bindBufferMemory2(bindBufferMemoryInfo);
    }
}
