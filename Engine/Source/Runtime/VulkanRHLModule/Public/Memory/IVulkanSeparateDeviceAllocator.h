#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include "IVulkanDeviceAllocator.h"
#include "VulkanDeviceMemory.h"

class IVulkanSeparateDeviceAllocator : public IVulkanDeviceAllocator {
public:
    [[nodiscard]] virtual VulkanDeviceMemory AllocateImageMemoryVkDedicated(vk::Image image) = 0;
    [[nodiscard]] virtual VulkanDeviceMemory AllocateBufferMemoryVkDedicated(vk::Buffer buffer) = 0;
};
