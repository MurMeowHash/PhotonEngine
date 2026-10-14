#pragma once

#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"

class VulkanRHLCommandList;
class VulkanRHLGraphicsWorkContext;

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
    [[nodiscard]] VulkanRHLTexture* GetBackBuffer() const;
    void UpdateViewportSize(uint32_t viewportWidth, uint32_t viewportHeight);
    void SetBlockUntilVBlank(bool isBlocked);
    void ReleaseActiveBackBuffer();
    void AdvanceBackBuffer(VulkanRHLGraphicsWorkContext* gfxWorkContext);
    void Present(VulkanRHLGraphicsWorkContext* gfxWorkContext);
private:
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanSurface* m_vulkanSurface = nullptr;
    VulkanSwapChain* m_vulkanSwapChain = nullptr;
    int32_t m_activeSwapChainImageIndex = -1;
    bool m_isBlockedUntilVBlank{};
    bool m_swapChainRecreateRequested{};
    std::vector<std::shared_ptr<VulkanBinarySemaphore>> m_renderFinishedSemaphores;

    uint32_t m_viewportWidth{};
    uint32_t m_viewportHeight{};

    [[nodiscard]] Photon::Result CreateRenderFinishedSemaphores(uint32_t semaCount);
    void ReleaseRenderFinishedSemaphores();
    bool RecreateSwapChain(VulkanRHLGraphicsWorkContext* gfxWorkContext);
    [[nodiscard]] bool TryGetActiveSwapChainImage(VulkanRHLTexture*& outSwapChainImage) const;
    [[nodiscard]] bool HasActiveSwapChainImage() const;
    void RequestSwapChainRecreate();
    void CloseSwapChainRecreateRequest();
};