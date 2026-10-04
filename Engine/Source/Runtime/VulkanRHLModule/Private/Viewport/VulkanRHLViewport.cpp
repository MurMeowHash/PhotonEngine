#include "../../Public/Viewport/VulkanRHLViewport.h"

VulkanRHLViewport::~VulkanRHLViewport() {
    delete m_vulkanSwapChain;
    delete m_vulkanSurface;
}

VulkanRHLViewport* VulkanRHLViewport::Create(const VulkanRHLViewportCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanSurfaceCreateInfo surfaceCreateInfo{};
    surfaceCreateInfo.m_vulkanInstance = createInfo.m_vulkanInstance;
    surfaceCreateInfo.m_windowHWND = createInfo.m_viewportInfo.m_windowHandle;

    InOutCreateParams<Photon::Result> surfaceInOutCreateParams{};
    VulkanSurface* surface = VulkanSurface::Create(surfaceCreateInfo, &surfaceInOutCreateParams);
    if (surfaceInOutCreateParams.m_result != Photon::Result::Success) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanSwapChainCreateInfo swapChainCreateInfo{};
    swapChainCreateInfo.m_vulkanDevice = createInfo.m_vulkanDevice;
    swapChainCreateInfo.m_vulkanSurface = surface;
    swapChainCreateInfo.m_width = createInfo.m_viewportInfo.m_viewportWidth;
    swapChainCreateInfo.m_height = createInfo.m_viewportInfo.m_viewportHeight;
    swapChainCreateInfo.m_blockUntilVBlank = createInfo.m_viewportInfo.m_blockUntilVBlank;
    swapChainCreateInfo.m_oldSwapChain = nullptr;

    InOutCreateParams<Photon::Result> swapChainInOutCreateParams{};
    VulkanSwapChain* swapChain = VulkanSwapChain::Create(swapChainCreateInfo, &swapChainInOutCreateParams);
    if (swapChainInOutCreateParams.m_result != Photon::Result::Success) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanRHLViewport* instance = Photon::AllocateObject<VulkanRHLViewport>(inOutCreateParams);
    instance->m_vulkanSurface = surface;
    instance->m_vulkanSwapChain = swapChain;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}