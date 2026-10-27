#pragma once

#define VULKAN_MAX_VERTEX_STREAMS 16

enum class VulkanRHLFormat {
    None = 0,
    B8G8R8A8_Srgb = 1,
    R32G32_Float = 2,
    R32G32B32_Float = 3,
};

enum class VulkanRHLVertexInputRate {
    PerVertex = 0,
    PerInstance = 1,
};