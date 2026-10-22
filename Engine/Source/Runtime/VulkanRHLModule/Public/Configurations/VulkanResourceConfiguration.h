#pragma once

#include "Resources/VulkanRHLResourceDefinitions.h"
#include <unordered_map>

namespace Photon::Vulkan::BufferPoolAllocationConfiguration {
    inline std::unordered_map<VulkanRHLBufferUpdateFrequency, vk::DeviceSize> g_bufferPoolMaxSize {
        {VulkanRHLBufferUpdateFrequency::Static, 1ul * 1024ul * 1024ul}
    };
}