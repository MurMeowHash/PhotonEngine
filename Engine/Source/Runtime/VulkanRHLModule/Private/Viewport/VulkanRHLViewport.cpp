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
    Photon::Result semaResult = instance->CreateRenderFinishedSemaphores(swapChain->GetImagesCount());
    Photon::PushResult(semaResult, inOutCreateParams);
    return instance;
}

Photon::Result VulkanRHLViewport::TryGetBackBuffer(VulkanRHLCommandList* rhlCommandList, VulkanRHLTexture *&backBuffer) {
    VulkanRHLTexture* activeSwapChainImage;
    if (TryGetActiveSwapChainImage(activeSwapChainImage)) {
        backBuffer = activeSwapChainImage;
        return Photon::Result::Success;
    }

    uint32_t swapChainImageIndex;
    std::shared_ptr<VulkanBinarySemaphore> acquireSemaphore{nullptr};
    VulkanSwapChainState swapChainState = m_vulkanSwapChain->TryAcquireNextImageIndex(swapChainImageIndex, acquireSemaphore);
    if (swapChainState == VulkanSwapChainState::OutOtDate || m_swapChainRecreateRequested) {
        RecreateSwapChain(rhlCommandList);
        return TryGetBackBuffer(rhlCommandList, backBuffer); // Hope it cant be out of date in every call, or I am fucked
    }

    if (!VulkanSwapChain::CanEverRender(swapChainState))
        return Photon::Result::UnknownFailure;

    rhlCommandList->GetGraphicsContext()->AddWaitSemaphore(acquireSemaphore, vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    m_activeSwapChainImageIndex = swapChainImageIndex;
    backBuffer = m_vulkanSwapChain->GetImageByIndex(swapChainImageIndex);
    return Photon::Result::Success;
}

void VulkanRHLViewport::UpdateViewportSize(uint32_t viewportWidth, uint32_t viewportHeight) {
    m_viewportWidth = viewportWidth;
    m_viewportHeight = viewportHeight;
    RequestSwapChainRecreate();
}

void VulkanRHLViewport::SetBlockUntilVBlank(bool isBlocked) {
    m_isBlockedUntilVBlank = isBlocked;
    RequestSwapChainRecreate();
}

void VulkanRHLViewport::ReleaseActiveBackBuffer() {
    m_activeSwapChainImageIndex = -1;
}

void VulkanRHLViewport::Present(VulkanRHLCommandList* rhlCommandList) {
    if (!HasActiveSwapChainImage())
        return;

    std::shared_ptr<VulkanBinarySemaphore> renderFinishedSemaphore = m_renderFinishedSemaphores[m_activeSwapChainImageIndex];
    rhlCommandList->GetGraphicsContext()->AddSignalSemaphore(renderFinishedSemaphore);
    rhlCommandList->Submit();

    VulkanSwapChainState swapChainState = m_vulkanSwapChain->Present(m_activeSwapChainImageIndex, renderFinishedSemaphore);
    if (swapChainState != VulkanSwapChainState::Healthy || m_swapChainRecreateRequested)
        RecreateSwapChain(rhlCommandList);

    ReleaseActiveBackBuffer();
}

Photon::Result VulkanRHLViewport::CreateRenderFinishedSemaphores(uint32_t semaCount) {
    m_renderFinishedSemaphores.reserve(semaCount);

    for (uint32_t i = 0; i < semaCount; i++) {
        std::shared_ptr<VulkanBinarySemaphore> acquireSemaphore{nullptr};
        Photon::Result acquireResult = m_vulkanDevice->TryAllocateSemaphore(acquireSemaphore);
        if (acquireResult != Photon::Result::Success)
            return acquireResult;

        m_renderFinishedSemaphores.emplace_back(acquireSemaphore);
    }

    return Photon::Result::Success;
}

void VulkanRHLViewport::ReleaseRenderFinishedSemaphores() {
    for (std::shared_ptr<VulkanBinarySemaphore>& semaphore : m_renderFinishedSemaphores)
        m_vulkanDevice->ReleaseSemaphore(semaphore);

    m_renderFinishedSemaphores.clear();
}

void VulkanRHLViewport::RecreateSwapChain(VulkanRHLCommandList* rhlCommandList) {
    CloseSwapChainRecreateRequest();
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
    ReleaseRenderFinishedSemaphores();
    CreateRenderFinishedSemaphores(m_vulkanSwapChain->GetImagesCount());
}

bool VulkanRHLViewport::TryGetActiveSwapChainImage(VulkanRHLTexture*& outSwapChainImage) const {
    return m_vulkanSwapChain->TryGetImageByIndex(m_activeSwapChainImageIndex, outSwapChainImage);
}

bool VulkanRHLViewport::HasActiveSwapChainImage() const {
    return m_activeSwapChainImageIndex != -1;
}

void VulkanRHLViewport::RequestSwapChainRecreate() {
    m_swapChainRecreateRequested = true;
}

void VulkanRHLViewport::CloseSwapChainRecreateRequest() {
    m_swapChainRecreateRequested = false;
}