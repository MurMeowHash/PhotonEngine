#pragma once

#include <vulkan/vulkan.hpp>

namespace Photon::Vulkan::BufferPoolAllocationConfiguration {
    inline constexpr vk::DeviceSize g_permanentBufferPageSize = 1ull * 1024ull * 1024ull;
    inline constexpr vk::DeviceSize g_temporaryBufferChunkSize = 1ull * 1024ull * 1024ull;
}