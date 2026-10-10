#include "../../Public/Viewport/VulkanRHLViewport.h"
#include "VulkanRHLCommandList.h"
#include "Synchronization/VulkanBinarySemaphore.h"

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
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    instance->m_vulkanSurface = surface;
    instance->m_vulkanSwapChain = swapChain;
    instance->m_isBlockedUntilVBlank = createInfo.m_viewportInfo.m_blockUntilVBlank;
    instance->m_viewportWidth = createInfo.m_viewportInfo.m_viewportWidth;
    instance->m_viewportHeight = createInfo.m_viewportInfo.m_viewportHeight;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

Photon::Result VulkanRHLViewport::TryGetBackBuffer(VulkanRHLCommandList* rhlCommandList, VulkanRHLTexture *&backBuffer) {
    if (m_activeSwapChainImage != nullptr) {
        backBuffer = m_activeSwapChainImage;
        return Photon::Result::Success;
    }

    VulkanRHLTexture* swapChainImage;
    VulkanBinarySemaphore* acquireSemaphore;
    VulkanSwapChainState swapChainState = m_vulkanSwapChain->TryAcquireNextImage(swapChainImage, acquireSemaphore);
    if (swapChainState == VulkanSwapChainState::OutOtDate) {
        RecreateSwapChain(rhlCommandList);
        return TryGetBackBuffer(rhlCommandList, backBuffer); // Hope it cant be out of date in every call, or I am fucked
    }

    if (!VulkanSwapChain::CanEverRender(swapChainState))
        return Photon::Result::UnknownFailure;

    rhlCommandList->GetGraphicsContext()->AddWaitSemaphore(acquireSemaphore, vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    m_activeSwapChainImage = swapChainImage;
    backBuffer = m_activeSwapChainImage;
    return Photon::Result::Success;
}

void VulkanRHLViewport::UpdateViewportSize(VulkanRHLCommandList* rhlCommandList, uint32_t viewportWidth, uint32_t viewportHeight) {
    m_viewportWidth = viewportWidth;
    m_viewportHeight = viewportHeight;
    RecreateSwapChain(rhlCommandList);
}

void VulkanRHLViewport::SetBlockUntilVBlank(VulkanRHLCommandList* rhlCommandList, bool isBlocked) {
    m_isBlockedUntilVBlank = isBlocked;
    RecreateSwapChain(rhlCommandList);
}

void VulkanRHLViewport::ReleaseActiveBackBuffer() {
    m_activeSwapChainImage = nullptr;
}

void VulkanRHLViewport::RecreateSwapChain(VulkanRHLCommandList* rhlCommandList) {
    rhlCommandList->Submit();
    ReleaseActiveBackBuffer();

    VulkanSwapChainCreateInfo createInfo;
    createInfo.m_vulkanDevice = m_vulkanDevice;
    createInfo.m_vulkanSurface = m_vulkanSurface;
    createInfo.m_width = m_viewportWidth;
    createInfo.m_height = m_viewportHeight;
    createInfo.m_blockUntilVBlank = m_isBlockedUntilVBlank;
    createInfo.m_oldSwapChain = m_vulkanSwapChain;

    InOutCreateParams<Photon::Result> inOut;
    VulkanSwapChain* newSwapChain = VulkanSwapChain::Create(createInfo, &inOut);
    if (inOut.m_result != Photon::Result::Success)
        return;

    delete m_vulkanSwapChain;
    m_vulkanSwapChain = newSwapChain;
}