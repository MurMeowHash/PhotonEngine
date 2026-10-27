#include "../Public/Shaders/Shader.h"
#include "CoreUtils.h"
#include "Logger.h"

GlobalShadersBlob::~GlobalShadersBlob() {
    for (std::pair<const ShaderDescriptor * const, VulkanRHLShader *>& shader: m_shaders) {
        delete shader.second;
    }
}

Photon::Result GlobalShadersBlob::PrepareShaders(VulkanRHLShaderCompiler *rhlShaderCompiler, VulkanDevice* vulkanDevice) {
    const std::vector<const ShaderDescriptor*>& globalShaderDescriptors = GlobalShaderDescriptorRegistry::GetGlobalShaderDescriptors();

    for (const ShaderDescriptor* shaderDescriptor : globalShaderDescriptors) {
        VulkanRHLShaderCompileInput compileInput{};
        compileInput.m_entryName = shaderDescriptor->m_entryName;
        compileInput.m_sourceFile = shaderDescriptor->m_sourceFile;
        compileInput.m_shaderStage = shaderDescriptor->m_stage;
        VulkanRHLShaderCompileOutput compileOutput{};
        Photon::Result shaderCompileResult = rhlShaderCompiler->CompileShader(compileInput, compileOutput);

        if (!compileOutput.m_errors.empty()) {
            Photon::Logger::PrintError(StringFormatter(shaderDescriptor->m_name, " compiled with errors: \n", compileOutput.m_errors) );
        }

        if (shaderCompileResult != Photon::Result::Success)
            return Photon::Result::UnknownFailure;

        Photon::Result shaderCreateResult = CreateVulkanRHLShader(vulkanDevice, shaderDescriptor, std::move(compileOutput.m_byteCode));
        if (shaderCreateResult != Photon::Result::Success)
            return shaderCreateResult;
    }

    return Photon::Result::Success;
}

Photon::Result GlobalShadersBlob::CreateVulkanRHLShader(VulkanDevice *vulkanDevice, const ShaderDescriptor *descriptor, std::vector<char> &&byteCode) {
    VulkanRHLTypedShaderCreateInfo createInfo{};
    createInfo.m_vulkanDevice = vulkanDevice;
    createInfo.m_shaderEntryName = descriptor->m_entryName;
    createInfo.m_spirVCode = VulkanSpirV(std::move(byteCode));
    InOutCreateParams<Photon::Result> inOut{};
    VulkanRHLShader* rhlShader{};

    switch (descriptor->m_stage) {
        case VulkanRHLShaderStage::Vertex:
            rhlShader = VulkanRHLVertexShader::Create(createInfo, &inOut);
            break;
        case VulkanRHLShaderStage::Fragment:
            rhlShader = VulkanRHLFragmentShader::Create(createInfo, &inOut);
            break;
        default:
            assert(false);
    }

    if (inOut.m_result != Photon::Result::Success)
        return inOut.m_result;

    m_shaders.emplace(descriptor, rhlShader);
    return Photon::Result::Success;
}