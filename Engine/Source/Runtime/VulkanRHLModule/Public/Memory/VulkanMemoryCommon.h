#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "VulkanDeviceMemory.h"

namespace Photon::VulkanMemoryCommon {
    VulkanDeviceMemory AllocateDeviceMemory(const vk::raii::Device& device, vk::DeviceSize memSize, uint32_t memoryTypeIndex,
        VkMemoryDedicatedAllocateInfo* dedicatedAllocationInfo = nullptr);
    void FreeDeviceMemory(const vk::raii::Device& device, VulkanDeviceMemory& memory);
    VulkanDeviceMemory AllocateDedicatedImageMemory(const vk::raii::Device& device, const vk::raii::Image& image, uint32_t memoryTypeIndex);
    VulkanDeviceMemory AllocateDedicatedBufferMemory(const vk::raii::Device& device, const vk::raii::Buffer& buffer, uint32_t memoryTypeIndex);
    vk::Result BindImageMemory(const vk::raii::Device& device, const VulkanDeviceMemory& memory, const vk::raii::Image& image);
    vk::Result BindBufferMemory(const vk::raii::Device& device, const VulkanDeviceMemory& memory, const vk::raii::Buffer& buffer);
};