#pragma once

#include "IVulkanRHLFactory.h"
#include "ModuleBase.h"
#include "VulkanDynamicRHL.h"
#include "Shaders/VulkanRHLShaderCompiler.h"

class VulkanRHLModule : public ModuleBase {
public:
    [[nodiscard]] Photon::Result StartUp() override;
    void Terminate() override;

    [[nodiscard]] VulkanDynamicRHL* GetVulkanDynamicRHL() const;
private:
    IVulkanRHLFactory* m_vulkanRHLFactory = nullptr;
    VulkanDynamicRHL* m_vulkanDynamicRHL = nullptr;
    VulkanRHLShaderCompiler* m_vulkanRHLShaderCompiler = nullptr;

    [[nodiscard]] Photon::Result CreateShaderCompiler();
};
