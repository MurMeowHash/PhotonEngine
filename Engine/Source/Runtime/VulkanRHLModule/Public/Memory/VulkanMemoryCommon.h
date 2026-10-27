#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "VulkanDeviceMemory.h"

class VulkanDevice;

namespace Photon::VulkanMemoryCommon {
    VulkanDeviceMemory AllocateDeviceMemory(VulkanDevice* vulkanDevice, vk::DeviceSize memSize, uint32_t memoryTypeIndex,
        VkMemoryDedicatedAllocateInfo* dedicatedAllocationInfo = nullptr);
    void FreeDeviceMemory(vk::Device device, VulkanDeviceMemory& memory);
    VulkanDeviceMemory AllocateDedicatedImageMemory(VulkanDevice* vulkanDevice, const vk::raii::Image& image, uint32_t memoryTypeIndex);
    VulkanDeviceMemory AllocateDedicatedBufferMemory(VulkanDevice* vulkanDevice, vk::Buffer buffer, uint32_t memoryTypeIndex);
    vk::Result BindImageMemory(const vk::raii::Device& device, const VulkanDeviceMemory& memory, const vk::raii::Image& image);
    vk::Result BindBufferMemory(vk::Device device, const VulkanDeviceMemory& memory, vk::Buffer buffer);
};