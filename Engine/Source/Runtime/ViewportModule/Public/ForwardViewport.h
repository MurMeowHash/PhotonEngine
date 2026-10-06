#pragma once

#include "Viewport.h"
#include "FactoryGlobals.h"
#include "CoreGlobals.h"
#include <windows.h>

class VulkanRHLViewport;

struct ForwardViewportCreateInfo {
    HWND m_viewportWindowHandle;
    uint32_t m_viewportWidth;
    uint32_t m_viewportHeight;
};

class ForwardViewport : public Viewport {
public:
    ~ForwardViewport() override;
    static ForwardViewport* Create(const ForwardViewportCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    VulkanRHLTexture* GetVulkanRHLTexture() override;
private:
    VulkanRHLViewport* m_vulkanRHLViewport = nullptr;
};