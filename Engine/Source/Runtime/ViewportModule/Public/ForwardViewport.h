#pragma once

#include "Viewport.h"
#include "FactoryGlobals.h"
#include "CoreGlobals.h"
#include <windows.h>

class VulkanRHLViewport;
class VulkanDynamicRHL;

struct ForwardViewportCreateInfo {
    HWND m_viewportWindowHandle;
    uint32_t m_viewportWidth;
    uint32_t m_viewportHeight;
};

class ForwardViewport : public Viewport {
public:
    VulkanRHLTexture* GetVulkanRHLTexture() override;
    void ChangeSize(uint32_t width, uint32_t height) override;
public:
    ~ForwardViewport() override;
    static ForwardViewport* Create(const ForwardViewportCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
private:
    VulkanRHLViewport* m_vulkanRHLViewport = nullptr;
    VulkanDynamicRHL* m_cachedDynamicRHL = nullptr;
};