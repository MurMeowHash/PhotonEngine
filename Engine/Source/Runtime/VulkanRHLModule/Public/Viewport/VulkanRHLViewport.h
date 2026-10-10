#pragma once

#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"

class VulkanRHLCommandList;

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
    [[nodiscard]] Photon::Result TryGetBackBuffer(VulkanRHLCommandList* rhlCommandList, VulkanRHLTexture*& backBuffer);
    void UpdateViewportSize(VulkanRHLCommandList* rhlCommandList, uint32_t viewportWidth, uint32_t viewportHeight);
    void SetBlockUntilVBlank(VulkanRHLCommandList* rhlCommandList, bool isBlocked);
    void ReleaseActiveBackBuffer();
private:
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanSurface* m_vulkanSurface = nullptr;
    VulkanSwapChain* m_vulkanSwapChain = nullptr;
    VulkanRHLTexture* m_activeSwapChainImage = nullptr;
    bool m_isBlockedUntilVBlank{};

    uint32_t m_viewportWidth{};
    uint32_t m_viewportHeight{};

    void RecreateSwapChain(VulkanRHLCommandList* rhlCommandList);
};