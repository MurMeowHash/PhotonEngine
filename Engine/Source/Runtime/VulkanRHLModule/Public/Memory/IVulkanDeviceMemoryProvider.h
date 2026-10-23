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

enum class VulkanDeviceMemoryAllocatorType {
    None = 0,
    Pooled = 1,
    Separate = 2,
};

struct VulkanDeviceMemoryAllocation {
    VulkanDeviceMemory m_memory{};
    VulkanResourceTilingType m_tiling = VulkanResourceTilingType::None;
    VulkanDeviceMemoryAllocatorType m_allocatorType = VulkanDeviceMemoryAllocatorType::None;

    [[nodiscard]] bool IsValid() const {
        return m_memory.IsValid();
    }
};

class IVulkanDeviceMemoryProvider {
public:
    virtual VulkanDeviceMemoryAllocation AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment, uint32_t memoryTypeIndex,
        VulkanResourceTilingType resourceTiling) = 0;
    virtual VulkanDeviceMemoryAllocation AllocateBufferMemory(vk::Buffer buffer, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) = 0;
    virtual VulkanDeviceMemoryAllocation AllocateImageMemory(vk::Image image, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags, VulkanResourceTilingType resourceTiling) = 0;
    [[nodiscard]] virtual bool TryFindMemoryTypeIndex(uint32_t memoryTypeRequirementsBits, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        uint32_t& memoryTypeIndex) = 0;
    virtual void FreeMemory(VulkanDeviceMemoryAllocation& allocation) = 0;
    virtual void FreeAllMemory() = 0;
    virtual ~IVulkanDeviceMemoryProvider() = default;
};