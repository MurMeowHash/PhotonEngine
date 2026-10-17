#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanGlobals.h"
#include "VulkanUtils.h"

class VulkanDevice;

struct VulkanRHLShaderCreateInfo {
    VulkanDevice* m_vulkanDevice;
    std::vector<char> m_spirVCode;
    Photon::Vulkan::RHLShaderStage m_shaderStage;
    const char* m_shaderEntryName;
};

struct VulkanRHLShaderMetaData {
    const char* m_entryName;
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
    [[nodiscard]] VulkanRHLShaderMetaData GetMetaData() const;
    [[nodiscard]] vk::ShaderStageFlagBits GetVulkanShaderStage() const;
private:
    vk::raii::ShaderModule m_handle = nullptr;
    Photon::Vulkan::RHLShaderStage m_shaderStage{};
    VulkanRHLShaderMetaData m_metaData{};
    vk::ShaderStageFlagBits m_vulkanShaderStage{};

    [[nodiscard]] Photon::Result CreateInternal(const VulkanRHLShaderCreateInfo& createInfo);
};

struct VulkanRHLTypedShaderCreateInfo {
    VulkanDevice* m_vulkanDevice;
    std::vector<char> m_spirVCode;
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