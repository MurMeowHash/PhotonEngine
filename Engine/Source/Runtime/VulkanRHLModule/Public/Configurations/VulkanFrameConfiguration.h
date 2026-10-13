#pragma once

#include <cstdint>

namespace Photon::Vulkan::FrameConfiguration {
    inline uint32_t g_maxAllowedGPUFrameLag = 2;
    inline uint32_t g_maxSwapChainAcquireRetries = 4;
}