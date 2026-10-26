#include "../../Public/Shaders/VulkanRHLShader.h"
#include "VulkanUtils.h"
#include "Device/VulkanDevice.h"
#include <memory>
#include "Shaders/VulkanShaderReflectModule.h"

vk::ShaderModule VulkanRHLShader::GetHandle() const {
    return *m_handle;
}

const VulkanShaderMetaData & VulkanRHLShader::GetMetaData() const {
    return m_metaData;
}

vk::ShaderStageFlagBits VulkanRHLShader::GetVulkanShaderStage() const {
    return m_vulkanShaderStage;
}

const char * VulkanRHLShader::GetEntryName() const {
    return m_entryName;
}

Photon::Result VulkanRHLShader::CreateInternal(const VulkanRHLShaderCreateInfo &createInfo) {
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = createInfo.m_spirVCode.GetCodeSize();
    shaderModuleCreateInfo.pCode = createInfo.m_spirVCode.GetCode();
    vk::ResultValue<vk::raii::ShaderModule> shaderModuleWrapper = createInfo.m_vulkanDevice->GetHandle().createShaderModule(shaderModuleCreateInfo);
    if (shaderModuleWrapper.result != vk::Result::eSuccess)
        return Photon::Result::UnknownFailure;

    m_handle = std::move(shaderModuleWrapper.value);
    m_shaderStage = createInfo.m_shaderStage;
    m_entryName = createInfo.m_shaderEntryName;
    m_vulkanShaderStage = Photon::Vulkan::RHLToVulkanShaderStage(createInfo.m_shaderStage);

    VulkanShaderReflectModuleCreateInfo reflectModuleCreateInfo{createInfo.m_spirVCode};
    InOutCreateParams<Photon::Result> reflectModuleInOut{};
    std::unique_ptr<VulkanShaderReflectModule> reflectModule = std::unique_ptr<VulkanShaderReflectModule>(
        VulkanShaderReflectModule::Create(reflectModuleCreateInfo, &reflectModuleInOut));

    if (reflectModuleInOut.m_result != Photon::Result::Success)
        return reflectModuleInOut.m_result;

    Photon::Result reflectResult = reflectModule->GenerateShaderMetaData(m_metaData);
    return reflectResult;
}

VulkanRHLVertexShader* VulkanRHLVertexShader::Create(const VulkanRHLTypedShaderCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLShaderCreateInfo shaderCreateInfo{
        createInfo.m_vulkanDevice,
        createInfo.m_spirVCode,
        VulkanRHLShaderStage::Vertex,
        createInfo.m_shaderEntryName
    };

    return VulkanRHLShader::Create<VulkanRHLVertexShader>(shaderCreateInfo, inOutCreateParams);
}

VulkanRHLFragmentShader* VulkanRHLFragmentShader::Create(const VulkanRHLTypedShaderCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLShaderCreateInfo shaderCreateInfo{
        createInfo.m_vulkanDevice,
        createInfo.m_spirVCode,
        VulkanRHLShaderStage::Fragment,
        createInfo.m_shaderEntryName
    };

    return VulkanRHLShader::Create<VulkanRHLFragmentShader>(shaderCreateInfo, inOutCreateParams);
}
