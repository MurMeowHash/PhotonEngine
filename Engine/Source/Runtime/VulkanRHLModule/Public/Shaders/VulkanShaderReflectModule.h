#pragma once

#include "VulkanShaderDefinitions.h"
#include "VulkanSpirV.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include <spirv_reflect.h>

struct VulkanShaderReflectModuleCreateInfo {
    VulkanSpirV m_spirV;
};

class VulkanShaderReflectModule {
public:
    ~VulkanShaderReflectModule();
    static VulkanShaderReflectModule* Create(const VulkanShaderReflectModuleCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    [[nodiscard]] Photon::Result GenerateShaderMetaData(VulkanShaderMetaData& outMetaData) const;
private:
    SpvReflectShaderModule m_handle{};

    [[nodiscard]] Photon::Result ReflectInputVariables(VulkanShaderMetaData& outMetaData) const;
    static VulkanRHLShaderSemantic TranslateSemanticToRHL(const char* semantic);
    static VulkanRHLShaderFormat TranslateReflectFormatToRHL(SpvReflectFormat spvFormat);
};