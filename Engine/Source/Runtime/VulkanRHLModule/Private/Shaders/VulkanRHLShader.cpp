#include "../../Public/Shaders/VulkanRHLShader.h"
#include "VulkanUtils.h"
#include "Device/VulkanDevice.h"

vk::ShaderModule VulkanRHLShader::GetHandle() const {
    return *m_handle;
}

VulkanRHLShaderMetaData VulkanRHLShader::GetMetaData() const {
    return m_metaData;
}

vk::ShaderStageFlagBits VulkanRHLShader::GetVulkanShaderStage() const {
    return m_vulkanShaderStage;
}

Photon::Result VulkanRHLShader::CreateInternal(const VulkanRHLShaderCreateInfo &createInfo) {
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = createInfo.m_spirVCode.size() * sizeof(char);
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(createInfo.m_spirVCode.data());
    vk::ResultValue<vk::raii::ShaderModule> shaderModuleWrapper = createInfo.m_vulkanDevice->GetHandle().createShaderModule(shaderModuleCreateInfo);
    if (shaderModuleWrapper.result != vk::Result::eSuccess)
        return Photon::Result::UnknownFailure;

    m_handle = std::move(shaderModuleWrapper.value);
    m_shaderStage = createInfo.m_shaderStage;
    m_metaData.m_entryName = createInfo.m_shaderEntryName;
    m_vulkanShaderStage = Photon::Vulkan::RHLToVulkanShaderStage(createInfo.m_shaderStage);
    return Photon::Result::Success;
}

VulkanRHLVertexShader* VulkanRHLVertexShader::Create(const VulkanRHLTypedShaderCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLShaderCreateInfo shaderCreateInfo{
        createInfo.m_vulkanDevice,
        createInfo.m_spirVCode,
        Photon::Vulkan::RHLShaderStage::Vertex,
        createInfo.m_shaderEntryName
    };

    return VulkanRHLShader::Create<VulkanRHLVertexShader>(shaderCreateInfo, inOutCreateParams);
}

VulkanRHLFragmentShader* VulkanRHLFragmentShader::Create(const VulkanRHLTypedShaderCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLShaderCreateInfo shaderCreateInfo{
        createInfo.m_vulkanDevice,
        createInfo.m_spirVCode,
        Photon::Vulkan::RHLShaderStage::Fragment,
        createInfo.m_shaderEntryName
    };

    return VulkanRHLShader::Create<VulkanRHLFragmentShader>(shaderCreateInfo, inOutCreateParams);
}
