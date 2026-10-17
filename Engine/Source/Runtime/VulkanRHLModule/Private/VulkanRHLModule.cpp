#include "../Public/VulkanRHLModule.h"
#include "VulkanRHLFactory.h"
#include "PlatformInteractor.h"
#include "Shaders/Shader.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

Photon::Result VulkanRHLModule::StartUp() {
    m_vulkanRHLFactory = new VulkanRHLFactory();
    InOutCreateParams<Photon::Result> rhlInOut{};
    m_vulkanDynamicRHL = m_vulkanRHLFactory->CreateVulkanDynamicRHL(VulkanDynamicRHLCreateInfo{}, &rhlInOut);
    if (rhlInOut.m_result != Photon::Result::Success)
        return rhlInOut.m_result;

    g_vulkanDynamicRHL = m_vulkanDynamicRHL;

    Photon::Result shaderCompilerResult = CreateShaderCompiler();
    if (shaderCompilerResult != Photon::Result::Success)
        return shaderCompilerResult;

    Photon::Result globalsShadersPrepareResult = CreateGlobalShaderBlob();
    Photon::Vulkan::g_vulkanRHLModule = this;
    return globalsShadersPrepareResult;
}

void VulkanRHLModule::Terminate() {
    delete m_vulkanRHLFactory;
    delete m_globalShadersBlob;
    delete m_vulkanRHLShaderCompiler;
    delete m_vulkanDynamicRHL;

    Photon::Vulkan::g_vulkanRHLModule = nullptr;
}

VulkanDynamicRHL * VulkanRHLModule::GetVulkanDynamicRHL() const {
    return m_vulkanDynamicRHL;
}

GlobalShadersBlob * VulkanRHLModule::GetGlobalShadersBlob() const {
    return m_globalShadersBlob;
}

Photon::Result VulkanRHLModule::CreateShaderCompiler() {
    VulkanRHLShaderCompilerCreateInfo createInfo{};
    createInfo.m_shadersRootDirectory = Photon::Core::PlatformInteractor::GetEngineDirectory() / SHADERS_DIR_NAME;
    createInfo.m_vulkanInstance = m_vulkanDynamicRHL->GetVulkanInstance();
    createInfo.m_vulkanDevice = m_vulkanDynamicRHL->GetVulkanDevice();
    InOutCreateParams<Photon::Result> shaderCompilerInOut{};
    m_vulkanRHLShaderCompiler = VulkanRHLShaderCompiler::Create(createInfo, &shaderCompilerInOut);
    return shaderCompilerInOut.m_result;
}

Photon::Result VulkanRHLModule::CreateGlobalShaderBlob() {
    m_globalShadersBlob = new GlobalShadersBlob();
    Photon::Result shadersPrepareResult = m_globalShadersBlob->PrepareShaders(m_vulkanRHLShaderCompiler, m_vulkanDynamicRHL->GetVulkanDevice());
    return shadersPrepareResult;
}