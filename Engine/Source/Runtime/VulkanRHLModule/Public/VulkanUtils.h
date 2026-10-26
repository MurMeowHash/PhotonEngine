#pragma once

#include <vulkan/vulkan.hpp>
#include "CoreUtils.h"
#include "VulkanGlobals.h"
#include "Resources/VulkanRHLResourceDefinitions.h"
#include "VulkanDefinitions.h"

namespace Photon::Vulkan {
    inline uint32_t EngineToVulkanVersion(Core::Version version) {
        return VK_MAKE_VERSION(version.m_major, version.m_minor, version.m_patch);
    }

    inline uint32_t EngineToVulkanApiVersion(Core::VulkanApiVersion apiVersion) {
        return VK_MAKE_API_VERSION(apiVersion.m_versionVariant, apiVersion.m_versionMajor,
            apiVersion.m_versionMinor, apiVersion.m_versionPatch);
    }

    inline vk::ShaderStageFlagBits RHLToVulkanShaderStage(VulkanRHLShaderStage rhlShaderStage) {
        switch (rhlShaderStage) {
            case VulkanRHLShaderStage::Vertex:
                return vk::ShaderStageFlagBits::eVertex;
            case VulkanRHLShaderStage::Fragment:
                return vk::ShaderStageFlagBits::eFragment;
            default:
                throw std::runtime_error("");
        }
    }

    inline vk::PrimitiveTopology RHLPrimitiveToVulkanTopology(RHLPrimitiveType primitiveType) {
        switch (primitiveType) {
            case RHLPrimitiveType::Triangle:
                return vk::PrimitiveTopology::eTriangleList;
            default:
                throw std::runtime_error("");
        }
    }

    inline vk::Format RHLFormatToVulkan(VulkanRHLFormat rhlFormat) {
        switch (rhlFormat) {
            case VulkanRHLFormat::B8G8R8A8_Srgb:
                return vk::Format::eB8G8R8A8Srgb;
            case VulkanRHLFormat::R32G32B32_Float:
                return vk::Format::eR32G32B32Sfloat;
            default:
                throw std::runtime_error("");
        }
    }

    inline vk::BufferUsageFlags RHLBufferUsageToVulkan(VulkanRHLBufferUsage bufferUsage) {
        vk::BufferUsageFlags bufferUsageFlags{};

        if (IS_FLAG_SET(VulkanRHLBufferUsage::VertexBuffer, bufferUsage))
            bufferUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;

        if (IS_FLAG_SET(VulkanRHLBufferUsage::IndexBuffer, bufferUsage))
            bufferUsageFlags |= vk::BufferUsageFlagBits::eIndexBuffer;

        return bufferUsageFlags;
    }

    inline vk::MemoryPropertyFlags ResolveVulkanMemoryFlagsFromUpdateFrequency(VulkanRHLBufferUpdateFrequency updateFrequency) {
        switch (updateFrequency) {
            case VulkanRHLBufferUpdateFrequency::Static:
                return vk::MemoryPropertyFlagBits::eDeviceLocal;
            default:
                throw std::runtime_error("");
        }
    }

    inline vk::IndexType GetIndexType(uint32_t elementSize) {
        if (elementSize == 1)
            return vk::IndexType::eUint8;

        if (elementSize == 2)
            return vk::IndexType::eUint16;

        return vk::IndexType::eUint32;
    }
}