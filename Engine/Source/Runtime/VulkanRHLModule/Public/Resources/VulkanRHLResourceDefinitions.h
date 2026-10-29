#pragma once

#include <cstdint>

enum class VulkanRHLBufferUpdateFrequency {
    None = 0,
    Static = 1,
    Volatile = 2,
};

enum class VulkanRHLBufferUsage : uint32_t {
    None = 0,
    VertexBuffer = 1 << 0,
    IndexBuffer = 1 << 1,

    All = VertexBuffer | IndexBuffer
};