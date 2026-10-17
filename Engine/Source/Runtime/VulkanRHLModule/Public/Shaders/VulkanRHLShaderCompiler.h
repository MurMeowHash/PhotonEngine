#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "VulkanGlobals.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

using Photon::Vulkan::RHLShaderStage;

class VulkanInstance;
class VulkanDevice;

struct IDxcUtils;
struct IDxcCompiler3;
struct IDxcIncludeHandler;

struct VulkanRHLShaderCompilerCreateInfo {
    VulkanInstance* m_vulkanInstance;
    VulkanDevice* m_vulkanDevice;
    std::filesystem::path m_shadersRootDirectory;
};

struct VulkanRHLShaderCompileInput {
    std::filesystem::path m_shaderPath;
    std::string m_entryName;
    RHLShaderStage m_shaderStage;
};

struct VulkanRHLShaderCompileOutput {
    std::vector<char> m_byteCode;
    std::string m_errors;
};

class VulkanRHLShaderCompiler {
public:
    ~VulkanRHLShaderCompiler();
    [[nodiscard]] static VulkanRHLShaderCompiler* Create(const VulkanRHLShaderCompilerCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] Photon::Result CompileShader(const VulkanRHLShaderCompileInput& shaderInput,
        VulkanRHLShaderCompileOutput& shaderCompileOutput) const;
private:
    std::filesystem::path m_shadersRootDirectory;
    std::wstring m_spirVTargetEnvironment;
    IDxcUtils* m_dxcUtils = nullptr;
    IDxcCompiler3* m_dxcCompiler = nullptr;
    IDxcIncludeHandler* m_dxcIncludeHandler = nullptr;

    [[nodiscard]] Photon::Result InitializeDxc();
    [[nodiscard]] static std::wstring GetSpirVTargetEnvironment(uint32_t vulkanApiVersion);
    [[nodiscard]] static const wchar_t* GetTargetProfile(RHLShaderStage shaderStage);
    [[nodiscard]] static const wchar_t* GetStageDefine(RHLShaderStage shaderStage);
};
