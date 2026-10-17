#pragma once

#include "VulkanGlobals.h"
#include <vector>
#include <unordered_map>
#include "VulkanRHLShader.h"
#include "VulkanRHLShaderCompiler.h"
#include "VulkanRHLModule.h"

using Photon::Vulkan::RHLShaderStage;

class VulkanDevice;

struct ShaderDescriptor {
    const char* m_name;
    const char* m_sourceFile;
    const char* m_entryName;
    RHLShaderStage m_stage;
};

struct GlobalShaderDescriptorRegistry {
    static std::vector<const ShaderDescriptor*>& GetGlobalShaderDescriptors() {
        static std::vector<const ShaderDescriptor*> globalShaderDescriptors;
        return globalShaderDescriptors;
    }
};

struct GlobalShaderDescriptorRegistrar {
    explicit GlobalShaderDescriptorRegistrar(const ShaderDescriptor& shaderDescriptor) {
        GlobalShaderDescriptorRegistry::GetGlobalShaderDescriptors().push_back(&shaderDescriptor);
    }
};

#define DECLARE_GLOBAL_SHADER()                                                                                     \
    public:                                                                                                         \
        static const ShaderDescriptor& GetStaticType();                                                             \
    private:                                                                                                        \
        static GlobalShaderDescriptorRegistrar s_shaderDescriptorRegistrar;

#define IMPLEMENT_GLOBAL_SHADER(ShaderClass, SourceFile, EntryName, Stage)                                          \
    const ShaderDescriptor& ShaderClass::GetStaticType() {                                                          \
        static ShaderDescriptor shaderDescriptor = ShaderDescriptor(#ShaderClass, SourceFile, EntryName, Stage);    \
        return shaderDescriptor;                                                                                    \
    }                                                                                                               \
                                                                                                                    \
    GlobalShaderDescriptorRegistrar ShaderClass::s_shaderDescriptorRegistrar(ShaderClass::GetStaticType());

template<typename TVulkanRHLShaderType>
class Shader {
public:
    using VulkanRHLShaderType = TVulkanRHLShaderType;
};

class ShadersBlob {
public:
    virtual ~ShadersBlob() = default;
    template<typename TShader>
    TShader::VulkanRHLShaderType* Resolve() const {
        return static_cast<TShader::VulkanRHLShaderType*>(m_shaders.at(&TShader::GetStaticType()));
    }

protected:
    std::unordered_map<const ShaderDescriptor*, VulkanRHLShader*> m_shaders;
};

class GlobalShadersBlob : public ShadersBlob {
public:
    ~GlobalShadersBlob() override;

    [[nodiscard]] Photon::Result PrepareShaders(VulkanRHLShaderCompiler* rhlShaderCompiler, VulkanDevice* vulkanDevice);

private:
    [[nodiscard]] Photon::Result CreateVulkanRHLShader(VulkanDevice* vulkanDevice, const ShaderDescriptor* descriptor, std::vector<char>&& byteCode);
};

#define GET_GLOBAL_SHADER(ShaderClass)    \
    Photon::Vulkan::g_vulkanRHLModule->GetGlobalShadersBlob()->Resolve<ShaderClass>()
