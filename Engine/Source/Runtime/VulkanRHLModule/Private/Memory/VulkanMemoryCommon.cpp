#include "../../Public/Memory/VulkanMemoryCommon.h"

namespace Photon::VulkanMemoryCommon {
    VulkanDeviceMemory AllocateDeviceMemory(vk::Device device, vk::DeviceSize memSize, uint32_t memoryTypeIndex,
        VkMemoryDedicatedAllocateInfo* dedicatedAllocationInfo) {
        VkMemoryAllocateInfo memoryAllocateInfo;
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memSize;
        memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
        memoryAllocateInfo.pNext = dedicatedAllocationInfo;
        VkDeviceMemory allocatedMemory;
        VkResult allocationResult = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &allocatedMemory);
        if (allocationResult != VK_SUCCESS)
            return VulkanDeviceMemory();

        VulkanDeviceMemoryInitializeInfo initializeInfo{};
        initializeInfo.m_device = device;
        initializeInfo.m_handle = allocatedMemory;
        initializeInfo.m_memoryTypeIndex = memoryTypeIndex;
        initializeInfo.m_dedicated = dedicatedAllocationInfo != nullptr
            && (dedicatedAllocationInfo->image != nullptr || dedicatedAllocationInfo->buffer != nullptr);
        initializeInfo.m_memInfo.m_size = memSize;
        initializeInfo.m_memInfo.m_offset = 0;
        initializeInfo.m_memInfo.m_alignmentOffset = 0;
        return VulkanDeviceMemory(initializeInfo);
    }

    void FreeDeviceMemory(vk::Device device, VulkanDeviceMemory &memory) {
        if (!memory.IsValid())
            return;

        vkFreeMemory(device, memory.GetHandle(), nullptr);
        memory.InvalidateMemory();
    }

    VulkanDeviceMemory AllocateDedicatedImageMemory(const vk::raii::Device &device, const vk::raii::Image& image,
        uint32_t memoryTypeIndex) {
        VkMemoryDedicatedAllocateInfo dedicatedMemoryAllocateInfo;
        dedicatedMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
        dedicatedMemoryAllocateInfo.image = *image;
        vk::MemoryRequirements imageMemoryRequirements = image.getMemoryRequirements();
        return AllocateDeviceMemory(device, imageMemoryRequirements.size, memoryTypeIndex, &dedicatedMemoryAllocateInfo);
    }

    VulkanDeviceMemory AllocateDedicatedBufferMemory(vk::Device device, vk::Buffer buffer,
        uint32_t memoryTypeIndex) {
        VkMemoryDedicatedAllocateInfo dedicatedMemoryAllocateInfo;
        dedicatedMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
        dedicatedMemoryAllocateInfo.buffer = buffer;
        vk::MemoryRequirements bufferMemoryRequirements;
        device.getBufferMemoryRequirements(buffer, &bufferMemoryRequirements);
        return AllocateDeviceMemory(device, bufferMemoryRequirements.size, memoryTypeIndex, &dedicatedMemoryAllocateInfo);
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
