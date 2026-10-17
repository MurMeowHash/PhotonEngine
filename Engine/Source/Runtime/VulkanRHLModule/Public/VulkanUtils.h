#pragma once

#include <vulkan/vulkan.hpp>

#include "VulkanGlobals.h"

namespace Photon::Vulkan {
    inline uint32_t EngineToVulkanVersion(Core::Version version) {
        return VK_MAKE_VERSION(version.m_major, version.m_minor, version.m_patch);
    }

    inline uint32_t EngineToVulkanApiVersion(Core::VulkanApiVersion apiVersion) {
        return VK_MAKE_API_VERSION(apiVersion.m_versionVariant, apiVersion.m_versionMajor,
            apiVersion.m_versionMinor, apiVersion.m_versionPatch);
    }

    inline vk::ShaderStageFlagBits RHLToVulkanShaderStage(RHLShaderStage rhlShaderStage) {
        switch (rhlShaderStage) {
            case RHLShaderStage::Vertex:
                return vk::ShaderStageFlagBits::eVertex;
            case RHLShaderStage::Fragment:
                return vk::ShaderStageFlagBits::eFragment;
            default:
                assert(false);
        }
    }

    inline vk::PrimitiveTopology RHLPrimitiveToVulkanTopology(RHLPrimitiveType primitiveType) {
        switch (primitiveType) {
            case RHLPrimitiveType::Triangle:
                return vk::PrimitiveTopology::eTriangleList;
            default:
                assert(false);
        }
    }

    inline vk::Format RHLFormatToVulkan(RHLFormat format) {
        switch (format) {
            case RHLFormat::B8G8R8A8:
                return vk::Format::eB8G8R8A8Srgb;
            default:
                assert(false);
        }
    }

    inline RHLFormat VulkanFormatToRHL(vk::Format format) {
        switch (format) {
            case vk::Format::eB8G8R8A8Srgb:
                return RHLFormat::B8G8R8A8;
            default:
                assert(false);
        }
    }
}