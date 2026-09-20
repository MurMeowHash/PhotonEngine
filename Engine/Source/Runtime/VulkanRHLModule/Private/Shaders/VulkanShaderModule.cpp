#include "../../Public/Shaders/VulkanShaderModule.h"

bool VulkanShaderModule::Create(const VulkanShaderModuleCreateInfo &createInfo) {
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = createInfo.m_spirVCode.size() * sizeof(char);
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(createInfo.m_spirVCode.data());
    vk::ResultValue<vk::raii::ShaderModule> shaderModuleWrapper = createInfo.m_vulkanDevice.createShaderModule(shaderModuleCreateInfo);
    if (shaderModuleWrapper.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(shaderModuleWrapper.value);
    m_stage = createInfo.m_shaderStage;
    m_entryName = createInfo.m_shaderEntryName;
    return true;
}

const vk::ShaderModule & VulkanShaderModule::GetHandle() const {
    return *m_handle;
}

vk::ShaderStageFlagBits VulkanShaderModule::GetStage() const {
    return m_stage;
}

const char * VulkanShaderModule::GetEntryName() const {
    return m_entryName;
}