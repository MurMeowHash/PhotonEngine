#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "VulkanDeviceMemory.h"

class IVulkanDedicatedDeviceAllocator {
public:
    virtual VulkanDeviceMemory AllocateImageMemory(const vk::raii::Image& image) = 0;
    virtual VulkanDeviceMemory AllocateBufferMemory(const vk::raii::Buffer& buffer) = 0;
    virtual void FreeMemory(VulkanDeviceMemory& memory) = 0;
    virtual void FreeAllMemory() = 0;
    virtual ~IVulkanDedicatedDeviceAllocator() = default;
};
