#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanGlobals.h"
#include "VulkanUtils.h"

struct VulkanRHLShaderCreateInfo {
    vk::raii::Device& m_vulkanDevice;
    std::vector<char> m_spirVCode;
    Photon::Vulkan::RHLShaderStage m_shaderStage;
    const char* m_shaderEntryName;
};

struct VulkanRHLShaderMetaData {
    const char* m_entryName;
};

class VulkanRHLShader {
public:
    template<std::derived_from<VulkanRHLShader> TVulkanShader>
    static TVulkanShader* Create(const VulkanRHLShaderCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) {
        vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
        shaderModuleCreateInfo.codeSize = createInfo.m_spirVCode.size() * sizeof(char);
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(createInfo.m_spirVCode.data());
        vk::ResultValue<vk::raii::ShaderModule> shaderModuleWrapper = createInfo.m_vulkanDevice.createShaderModule(shaderModuleCreateInfo);
        if (shaderModuleWrapper.result != vk::Result::eSuccess) {
            Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
            return nullptr;
        }

        TVulkanShader* instance = Photon::AllocateObject<TVulkanShader>(inOutCreateParams);
        instance->m_handle = std::move(shaderModuleWrapper.value);
        instance->m_shaderStage = createInfo.m_shaderStage;
        instance->m_entryName = createInfo.m_shaderEntryName;
        instance->m_vulkanShaderStage = Photon::Vulkan::RHLToVulkanShaderStage(createInfo.m_shaderStage);
        Photon::PushResult(Photon::Result::Success, inOutCreateParams);
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
};

struct VulkanRHLTypedShaderCreateInfo {
    vk::raii::Device& m_vulkanDevice;
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