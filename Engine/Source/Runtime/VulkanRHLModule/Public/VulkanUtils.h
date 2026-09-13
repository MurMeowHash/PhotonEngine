#pragma once

#include <vulkan/vulkan.hpp>

namespace Photon::Vulkan {
    inline uint32_t EngineToVulkanVersion(Core::Version version) {
        return VK_MAKE_VERSION(version.m_major, version.m_minor, version.m_patch);
    }

    inline uint32_t EngineToVulkanApiVersion(Core::VulkanApiVersion apiVersion) {
        return VK_MAKE_API_VERSION(apiVersion.m_versionVariant, apiVersion.m_versionMajor,
            apiVersion.m_versionMinor, apiVersion.m_versionPatch);
    }
}