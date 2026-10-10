#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

class VulkanDynamicRHL;

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

    enum class ImageType {
        None = 0,
        p1D = 1,
        p2D = 2,
        p3D = 3,
        pCubeMap = 4,
    };

    enum class RHLWorkType {
        Graphics = 1,
    };

    inline VulkanDynamicRHL* g_vulkanDynamicRHL = nullptr;
}