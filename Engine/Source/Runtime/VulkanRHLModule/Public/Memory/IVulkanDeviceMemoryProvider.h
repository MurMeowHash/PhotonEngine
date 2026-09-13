#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "VulkanDeviceMemory.h"
#include "VulkanResourceTilingType.h"

enum class DedicatedMemoryRequirementsFlags : uint32_t {
    None = 0,
    ForceDedicated = 1 << 0,
    UseDedicatedIfPreferred = 1 << 1,
};

class IVulkanDeviceMemoryProvider {
public:
    virtual VulkanDeviceMemory AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment, uint32_t memoryTypeIndex,
        VulkanResourceTilingType resourceTiling) = 0;
    virtual VulkanDeviceMemory AllocateBufferMemory(const vk::raii::Buffer& buffer, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) = 0;
    virtual VulkanDeviceMemory AllocateImageMemory(const vk::raii::Image &image, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags, VulkanResourceTilingType resourceTiling) = 0;
    [[nodiscard]] virtual bool TryFindMemoryTypeIndex(uint32_t memoryTypeRequirementsBits, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        uint32_t& memoryTypeIndex) = 0;
    virtual void FreeMemory(VulkanDeviceMemory& deviceMemory) = 0;
    virtual void FreeAllMemory() = 0;
    virtual ~IVulkanDeviceMemoryProvider() = default;
};