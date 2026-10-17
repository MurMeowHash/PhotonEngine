#pragma once

#include "IVulkanRHLFactory.h"
#include "ModuleBase.h"
#include "VulkanDynamicRHL.h"
#include "Shaders/VulkanRHLShaderCompiler.h"

class GlobalShadersBlob;

class VulkanRHLModule : public ModuleBase {
public:
    [[nodiscard]] Photon::Result StartUp() override;
    void Terminate() override;

    [[nodiscard]] VulkanDynamicRHL* GetVulkanDynamicRHL() const;
    [[nodiscard]] GlobalShadersBlob* GetGlobalShadersBlob() const;
private:
    IVulkanRHLFactory* m_vulkanRHLFactory = nullptr;
    VulkanDynamicRHL* m_vulkanDynamicRHL = nullptr;
    VulkanRHLShaderCompiler* m_vulkanRHLShaderCompiler = nullptr;
    GlobalShadersBlob* m_globalShadersBlob = nullptr;

    [[nodiscard]] Photon::Result CreateShaderCompiler();
    [[nodiscard]] Photon::Result CreateGlobalShaderBlob();
};
