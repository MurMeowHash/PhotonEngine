#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

struct VulkanShaderModuleCreateInfo {
    vk::raii::Device& m_vulkanDevice;
    std::vector<char> m_spirVCode;
    vk::ShaderStageFlagBits m_shaderStage;
    const char* m_shaderEntryName;
};

class VulkanShaderModule {
public:
    [[nodiscard]] bool Create(const VulkanShaderModuleCreateInfo& createInfo);
    [[nodiscard]] const vk::ShaderModule& GetHandle() const;
    [[nodiscard]] vk::ShaderStageFlagBits GetStage() const;
    [[nodiscard]] const char* GetEntryName() const;
private:
    vk::raii::ShaderModule m_handle = nullptr;
    vk::ShaderStageFlagBits m_stage = vk::ShaderStageFlagBits::eAll;
    const char* m_entryName = "";
};