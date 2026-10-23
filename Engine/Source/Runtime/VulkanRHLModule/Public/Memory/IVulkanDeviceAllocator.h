#pragma once

#include "VulkanDeviceMemory.h"

class IVulkanDeviceAllocator {
public:
    [[nodiscard]] virtual VulkanDeviceMemory AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment) = 0;
    [[nodiscard]] virtual vk::DeviceSize GetMaxAllocationSize() const = 0;
    virtual void FreeMemory(VulkanDeviceMemory& memory) = 0;
    virtual void FreeAllMemory() = 0;
    virtual ~IVulkanDeviceAllocator() = default;
};
