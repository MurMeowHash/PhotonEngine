#include "../../Public/Shaders/VulkanRHLShaderCompiler.h"
#include "Instance/VulkanInstance.h"
#include "Device/VulkanDevice.h"

#include <windows.h>
#include <cstddef>
#include <cstdint>

namespace {
    constexpr uint8_t HexDigitValue(char hexDigit) {
        if (hexDigit >= '0' && hexDigit <= '9')
            return static_cast<uint8_t>(hexDigit - '0');

        if (hexDigit >= 'a' && hexDigit <= 'f')
            return static_cast<uint8_t>(hexDigit - 'a' + 10);

        return static_cast<uint8_t>(hexDigit - 'A' + 10);
    }

    constexpr uint32_t ParseHexNumber(const char* hexText, size_t digitCount) {
        uint32_t value = 0;
        for (size_t i = 0; i < digitCount; i++)
            value = value * 16 + HexDigitValue(hexText[i]);

        return value;
    }

    constexpr IID ParseInterfaceId(const char* interfaceIdText) {
        IID interfaceId{};
        interfaceId.Data1 = ParseHexNumber(interfaceIdText, 8);
        interfaceId.Data2 = static_cast<unsigned short>(ParseHexNumber(interfaceIdText + 9, 4));
        interfaceId.Data3 = static_cast<unsigned short>(ParseHexNumber(interfaceIdText + 14, 4));
        interfaceId.Data4[0] = static_cast<unsigned char>(ParseHexNumber(interfaceIdText + 19, 2));
        interfaceId.Data4[1] = static_cast<unsigned char>(ParseHexNumber(interfaceIdText + 21, 2));
        for (size_t i = 0; i < 6; i++)
            interfaceId.Data4[2 + i] = static_cast<unsigned char>(ParseHexNumber(interfaceIdText + 24 + i * 2, 2));

        return interfaceId;
    }
}

template<typename TDxcInterface>
static constexpr IID g_dxcInterfaceId{};

#define CROSS_PLATFORM_UUIDOF(interface, spec)                                          \
    struct interface;                                                                   \
    template<> constexpr IID g_dxcInterfaceId<interface> = ParseInterfaceId(spec);

#ifndef _Maybenull_
#define _Maybenull_
#endif
#include <dxcapi.h>

namespace {
    template<typename TDxcObject>
    void ReleaseDxcObject(TDxcObject*& dxcObject) {
        if (dxcObject == nullptr)
            return;

        dxcObject->Release();
        dxcObject = nullptr;
    }
}

VulkanRHLShaderCompiler::~VulkanRHLShaderCompiler() {
    ReleaseDxcObject(m_dxcIncludeHandler);
    ReleaseDxcObject(m_dxcCompiler);
    ReleaseDxcObject(m_dxcUtils);
}

VulkanRHLShaderCompiler* VulkanRHLShaderCompiler::Create(const VulkanRHLShaderCompilerCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLShaderCompiler* instance = Photon::AllocateObject<VulkanRHLShaderCompiler>(inOutCreateParams);
    instance->m_shadersRootDirectory = createInfo.m_shadersRootDirectory;
    uint32_t vulkanApiVersion = (std::min)(createInfo.m_vulkanInstance->GetApiVersion(), createInfo.m_vulkanDevice->GetApiVersion());
    instance->m_spirVTargetEnvironment = GetSpirVTargetEnvironment(vulkanApiVersion);
    Photon::Result dxcResult = instance->InitializeDxc();
    Photon::PushResult(dxcResult, inOutCreateParams);
    return instance;
}

Photon::Result VulkanRHLShaderCompiler::CompileShader(const VulkanRHLShaderCompileInput &shaderInput,
    VulkanRHLShaderCompileOutput &shaderCompileOutput) const {
    std::filesystem::path fullShaderPath = m_shadersRootDirectory / shaderInput.m_sourceFile;
    std::wstring sourcePath = fullShaderPath.wstring();

    IDxcBlobEncoding* sourceBlob = nullptr;
    if (FAILED(m_dxcUtils->LoadFile(sourcePath.c_str(), nullptr, &sourceBlob))) {
        shaderCompileOutput.m_errors = "Failed to read shader file: " + fullShaderPath.string();
        return Photon::Result::UnknownFailure;
    }

    DxcBuffer sourceBuffer{};
    sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
    sourceBuffer.Size = sourceBlob->GetBufferSize();
    sourceBuffer.Encoding = DXC_CP_ACP;

    std::wstring entryName(shaderInput.m_entryName.begin(), shaderInput.m_entryName.end());
    std::wstring targetEnvironment = L"-fspv-target-env=" + m_spirVTargetEnvironment;
    std::wstring includeDirectory = m_shadersRootDirectory.wstring();
    std::vector<LPCWSTR> arguments = {
        sourcePath.c_str(),
        L"-E", entryName.c_str(),
        L"-T", GetTargetProfile(shaderInput.m_shaderStage),
        L"-D", GetStageDefine(shaderInput.m_shaderStage),
        L"-I", includeDirectory.c_str(),
        L"-spirv",
        L"-fspv-reflect",
        targetEnvironment.c_str(),
    };

    IDxcResult* compileResult = nullptr;
    HRESULT compileCallResult = m_dxcCompiler->Compile(&sourceBuffer, arguments.data(), static_cast<UINT32>(arguments.size()),
        m_dxcIncludeHandler, g_dxcInterfaceId<IDxcResult>, reinterpret_cast<void**>(&compileResult));
    ReleaseDxcObject(sourceBlob);
    if (FAILED(compileCallResult))
        return Photon::Result::UnknownFailure;

    IDxcBlobUtf8* errorsBlob = nullptr;
    compileResult->GetOutput(DXC_OUT_ERRORS, g_dxcInterfaceId<IDxcBlobUtf8>, reinterpret_cast<void**>(&errorsBlob), nullptr);
    if (errorsBlob != nullptr && errorsBlob->GetStringLength() > 0)
        shaderCompileOutput.m_errors.assign(errorsBlob->GetStringPointer(), errorsBlob->GetStringLength());

    ReleaseDxcObject(errorsBlob);

    HRESULT compileStatus;
    compileResult->GetStatus(&compileStatus);
    if (FAILED(compileStatus)) {
        ReleaseDxcObject(compileResult);
        return Photon::Result::UnknownFailure;
    }

    IDxcBlob* byteCodeBlob = nullptr;
    compileResult->GetOutput(DXC_OUT_OBJECT, g_dxcInterfaceId<IDxcBlob>, reinterpret_cast<void**>(&byteCodeBlob), nullptr);
    if (byteCodeBlob == nullptr) {
        ReleaseDxcObject(compileResult);
        return Photon::Result::UnknownFailure;
    }

    const char* byteCode = static_cast<const char*>(byteCodeBlob->GetBufferPointer());
    shaderCompileOutput.m_byteCode.assign(byteCode, byteCode + byteCodeBlob->GetBufferSize());

    ReleaseDxcObject(byteCodeBlob);
    ReleaseDxcObject(compileResult);
    return Photon::Result::Success;
}

Photon::Result VulkanRHLShaderCompiler::InitializeDxc() {
    if (FAILED(DxcCreateInstance(CLSID_DxcUtils, g_dxcInterfaceId<IDxcUtils>, reinterpret_cast<void**>(&m_dxcUtils))))
        return Photon::Result::UnknownFailure;

    if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, g_dxcInterfaceId<IDxcCompiler3>, reinterpret_cast<void**>(&m_dxcCompiler))))
        return Photon::Result::UnknownFailure;

    if (FAILED(m_dxcUtils->CreateDefaultIncludeHandler(&m_dxcIncludeHandler)))
        return Photon::Result::UnknownFailure;

    return Photon::Result::Success;
}

std::wstring VulkanRHLShaderCompiler::GetSpirVTargetEnvironment(uint32_t vulkanApiVersion) {
    uint32_t majorVersion = VK_API_VERSION_MAJOR(vulkanApiVersion);
    uint32_t minorVersion = VK_API_VERSION_MINOR(vulkanApiVersion);
    if (majorVersion > 1 || minorVersion >= 3)
        return L"vulkan1.3";

    return L"vulkan1." + std::to_wstring(minorVersion);
}

const wchar_t* VulkanRHLShaderCompiler::GetTargetProfile(VulkanRHLShaderStage shaderStage) {
    switch (shaderStage) {
        case VulkanRHLShaderStage::Vertex:
            return L"vs_6_0";
        case VulkanRHLShaderStage::Fragment:
            return L"ps_6_0";
        default:
            return L"";
    }
}

const wchar_t* VulkanRHLShaderCompiler::GetStageDefine(VulkanRHLShaderStage shaderStage) {
    switch (shaderStage) {
        case VulkanRHLShaderStage::Vertex:
            return L"VERTEXSHADER=1";
        case VulkanRHLShaderStage::Fragment:
            return L"PIXELSHADER=1";
        default:
            return L"";
    }
}
