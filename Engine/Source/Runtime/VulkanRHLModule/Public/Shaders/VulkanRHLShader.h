#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanGlobals.h"
#include "VulkanShaderDefinitions.h"
#include "VulkanSpirV.h"

class VulkanDevice;

struct VulkanRHLShaderCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanSpirV m_spirVCode;
    VulkanRHLShaderStage m_shaderStage;
    const char* m_shaderEntryName;
};

class VulkanRHLShader {
public:
    virtual ~VulkanRHLShader() = default;
    template<std::derived_from<VulkanRHLShader> TVulkanShader>
    static TVulkanShader* Create(const VulkanRHLShaderCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) {
        TVulkanShader* instance = Photon::AllocateObject<TVulkanShader>(inOutCreateParams);
        Photon::Result createResult = instance->CreateInternal(createInfo);
        Photon::PushResult(createResult, inOutCreateParams);
        return instance;
    }

    [[nodiscard]] vk::ShaderModule GetHandle() const;
    [[nodiscard]] const VulkanShaderMetaData& GetMetaData() const;
    [[nodiscard]] vk::ShaderStageFlagBits GetVulkanShaderStage() const;
    [[nodiscard]] const char* GetEntryName() const;
private:
    vk::raii::ShaderModule m_handle = nullptr;
    VulkanRHLShaderStage m_shaderStage{};
    vk::ShaderStageFlagBits m_vulkanShaderStage{};
    VulkanShaderMetaData m_metaData{};
    const char* m_entryName{};

    [[nodiscard]] Photon::Result CreateInternal(const VulkanRHLShaderCreateInfo& createInfo);
};

struct VulkanRHLTypedShaderCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanSpirV m_spirVCode;
    const char* m_shaderEntryName;
};

class VulkanRHLVertexShader : public VulkanRHLShader {
public:
    static VulkanRHLVertexShader* Create(const VulkanRHLTypedShaderCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
};

class VulkanRHLFragmentShader : public VulkanRHLShader {
public:
    static VulkanRHLFragmentShader* Create(const VulkanRHLTypedShaderCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
};