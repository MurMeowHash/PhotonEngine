#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Memory/VulkanDeviceMemory.h"

enum class VulkanRHLBufferUpdateFrequency {
    Static = 0,
};



struct VulkanRHLBufferCreateInfo {enum class VulkanRHLBufferUsage : uint32_t {
    None = 0,
    VertexBuffer = 1 << 0,
    IndexBuffer = 1 << 1,
};
    uint64_t m_bufferSize;
    VulkanRHLBufferUpdateFrequency m_updateFrequency;
    VulkanRHLBufferUsage m_usage;
};

struct VulkanBufferSlice {
    VulkanDeviceMemory m_memory;
    vk::Buffer m_handle;
};

class VulkanRHLBuffer {
public:
    static VulkanRHLBuffer* Create(const VulkanRHLBufferCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
private:
    VulkanBufferSlice* m_activeBufferSlice = nullptr;

    [[nodiscard]] static vk::BufferUsageFlags GetVulkanUsageFromPhoton(VulkanRHLBufferUsage usage);
};