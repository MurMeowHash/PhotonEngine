#pragma once

#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"

struct RHLViewportCreateInfo {
    HWND m_windowHandle;
    uint32_t m_viewportWidth;
    uint32_t m_viewportHeight;
    bool m_blockUntilVBlank;
};

struct VulkanRHLViewportCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanInstance* m_vulkanInstance;
    RHLViewportCreateInfo m_viewportInfo;
};

class VulkanRHLViewport {
public:
    ~VulkanRHLViewport();
    static VulkanRHLViewport* Create(const VulkanRHLViewportCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
private:
    VulkanSurface* m_vulkanSurface = nullptr;
    VulkanSwapChain* m_vulkanSwapChain = nullptr;
};