#include "../../Public/Shaders/VulkanRHLShader.h"
#include "VulkanUtils.h"

vk::ShaderModule VulkanRHLShader::GetHandle() const {
    return *m_handle;
}

VulkanRHLShaderMetaData VulkanRHLShader::GetMetaData() const {
    return m_metaData;
}

vk::ShaderStageFlagBits VulkanRHLShader::GetVulkanShaderStage() const {
    return m_vulkanShaderStage;
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
