#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

#define SHADERS_DIR_NAME "Shaders"

class VulkanDynamicRHL;
class VulkanRHLCommandBuffer;
class VulkanRHLModule;

namespace Photon::Vulkan {
    struct VulkanApiVersion {
        int m_versionVariant;
        int m_versionMajor;
        int m_versionMinor;
        int m_versionPatch;

        [[nodiscard]] uint32_t ToVulkanVersion() const {
            return VK_MAKE_API_VERSION(m_versionVariant, m_versionMajor, m_versionMinor, m_versionPatch);
        }
    };

    enum class DeviceSearchFlags : uint32_t {
        None = 0,
        AllowNonGpu = 1u << 0,
        RenderingOnly = 1u << 1,
    };

    enum class RHLPipeline : uint32_t {
        None = 0,
        Graphics = 1u << 0,
        DedicatedCompute = 1u << 1,
        DedicatedTransfer = 1u << 2,
    };

    enum class ImageType {
        None = 0,
        p1D = 1,
        p2D = 2,
        p3D = 3,
        pCubeMap = 4,
    };

    enum class RHLPipelineUsage {
        DontCare = 0,
        Unknown = 1,
        RenderTarget = 2,
        Present = 3,
    };

    enum class RHLResourceType {
        Texture = 0,
        Buffer = 1,
    };

    enum class RHLShaderStage {
        None = 0,
        Vertex = 1,
        Fragment = 2,
    };

    enum class RHLPrimitiveType {
        None = 0,
        Triangle = 1,
    };

    enum class RHLFormat {
        None = 0,
        B8G8R8A8 = 1,
    };

    inline VulkanDynamicRHL* g_vulkanDynamicRHL = nullptr;
    inline VulkanRHLModule* g_vulkanRHLModule = nullptr;
}

struct VulkanRHLRect {
    uint32_t m_minX;
    uint32_t m_minY;
    uint32_t m_maxX;
    uint32_t m_maxY;
};