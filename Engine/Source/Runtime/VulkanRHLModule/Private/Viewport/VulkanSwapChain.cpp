#include "../../Public/Viewport/VulkanSwapChain.h"
#include "Device/VulkanDevice.h"
#include "Synchronization/VulkanBinarySemaphore.h"
#include "Viewport/VulkanSurface.h"

VulkanSwapChain::~VulkanSwapChain() {
    m_vulkanDevice->WaitIdle();

    for (VulkanBinarySemaphore* semaphore : m_acquireSemaphores)
        delete semaphore;

    (*m_vulkanDevice->GetHandle()).destroySwapchainKHR(m_handle);
}

VulkanSwapChain* VulkanSwapChain::Create(const VulkanSwapChainCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::ResultValue<vk::SurfaceCapabilitiesKHR> surfaceProperties =
        createInfo.m_vulkanDevice->GetPhysicalHandle().getSurfaceCapabilitiesKHR(*createInfo.m_vulkanSurface->GetHandle());

    if (surfaceProperties.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.surface = *createInfo.m_vulkanSurface->GetHandle();
    swapChainCreateInfo.minImageCount = GetSwapChainMinImageCount(surfaceProperties.value);

    vk::SurfaceFormatKHR surfaceFormat{};
    if (!TryGetSurfaceFormat(createInfo.m_vulkanDevice, createInfo.m_vulkanSurface, surfaceFormat)) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    const uint32_t chainArrayLayers = 1;

    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = GetClampedExtents(createInfo.m_width, createInfo.m_height, surfaceProperties.value);
    swapChainCreateInfo.imageArrayLayers = chainArrayLayers;
    swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapChainCreateInfo.preTransform = surfaceProperties->currentTransform;
    swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapChainCreateInfo.presentMode = GetPresentMode(createInfo.m_vulkanDevice, createInfo.m_vulkanSurface, createInfo.m_blockUntilVBlank);
    swapChainCreateInfo.clipped = vk::True;
    swapChainCreateInfo.oldSwapchain = createInfo.m_oldSwapChain == nullptr ? nullptr : createInfo.m_oldSwapChain->GetHandle();

    vk::ResultValue<vk::SwapchainKHR> swapChainWrapper =
        (*createInfo.m_vulkanDevice->GetHandle()).createSwapchainKHR(swapChainCreateInfo);

    if (swapChainWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanSwapChain* instance = Photon::AllocateObject<VulkanSwapChain>(inOutCreateParams);
    instance->m_handle = swapChainWrapper.value;
    instance->m_format = surfaceFormat.format;
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    Photon::Result imageRetrieveResult = instance->RetrieveImages(chainArrayLayers);
    if (imageRetrieveResult == Photon::Result::Success)
        imageRetrieveResult = instance->CreateAcquireSemaphores(instance->m_images.size());

    Photon::PushResult(imageRetrieveResult, inOutCreateParams);
    return instance;
}

vk::SwapchainKHR VulkanSwapChain::GetHandle() const {
    return m_handle;
}

VulkanSwapChainState VulkanSwapChain::TryAcquireNextImage(VulkanRHLTexture *&swapChainImage, VulkanBinarySemaphore *&acquireSemaphore) {
    m_currentAcquireImageIndex = (m_currentAcquireImageIndex + 1) % m_images.size();
    acquireSemaphore = m_acquireSemaphores[m_currentAcquireImageIndex];
    VulkanSemaphoreLockResult acquireSemaphoreLock = acquireSemaphore->ScheduleAcquire();
    vk::ResultValue<uint32_t> acquiredImageWrapper = (*m_vulkanDevice->GetHandle()).acquireNextImageKHR(m_handle, UINT64_MAX, acquireSemaphoreLock.m_semaphoreHandle);
    VulkanSwapChainState swapChainState = ResolveSwapChainStateByVulkanResult(acquiredImageWrapper.result);
    if (!CanEverRender(swapChainState))
        return swapChainState;

    swapChainImage = m_images[acquiredImageWrapper.value];
    return swapChainState;
}

Photon::Result VulkanSwapChain::RetrieveImages(uint32_t arrayLayers) {
    vk::ResultValue<std::vector<vk::Image>> imageRetrieveResult = (*m_vulkanDevice->GetHandle()).getSwapchainImagesKHR(m_handle);
    if (imageRetrieveResult.result != vk::Result::eSuccess)
        return Photon::Result::UnknownFailure;

    m_images.reserve(imageRetrieveResult.value.size());
    VulkanImageCreateInfo createInfo;
    createInfo.m_vulkanDevice = m_vulkanDevice;
    createInfo.m_imageFormat = m_format;
    createInfo.m_imageType = Photon::Vulkan::ImageType::p2D;
    createInfo.m_arrayLayers = arrayLayers;
    createInfo.m_mipLevels = 1;
    for (vk::Image rawImage: imageRetrieveResult.value) {
        createInfo.m_imageHandle = rawImage;
        m_images.emplace_back(VulkanRHLTexture::Create(createInfo));
    }

    return Photon::Result::Success;
}

Photon::Result VulkanSwapChain::CreateAcquireSemaphores(uint32_t semaphoresCount) {
    m_acquireSemaphores.reserve(semaphoresCount);

    VulkanBinarySemaphoreCreateInfo createInfo;
    createInfo.m_vulkanDevice = m_vulkanDevice;
    InOutCreateParams<Photon::Result> inOut;

    for (uint32_t i = 0; i < semaphoresCount; i++) {
        VulkanBinarySemaphore* semaphore = VulkanBinarySemaphore::Create(createInfo, &inOut);
        if (inOut.m_result == Photon::Result::UnknownFailure)
            return inOut.m_result;

        m_acquireSemaphores.emplace_back(semaphore);
    }

    return Photon::Result::Success;
}

uint32_t VulkanSwapChain::GetSwapChainMinImageCount(const vk::SurfaceCapabilitiesKHR &surfaceProperties) {
    uint32_t desiredImageCount = surfaceProperties.minImageCount + 1;
    return surfaceProperties.maxImageCount > 0
    ? std::clamp(desiredImageCount, surfaceProperties.minImageCount, surfaceProperties.maxImageCount)
    : desiredImageCount;
}

bool VulkanSwapChain::TryGetSurfaceFormat(VulkanDevice *vulkanDevice, VulkanSurface* vulkanSurface, VkSurfaceFormatKHR &surfaceFormat) {
    vk::ResultValue<std::vector<vk::SurfaceFormatKHR>> surfaceFormats =
        vulkanDevice->GetPhysicalHandle().getSurfaceFormatsKHR(*vulkanSurface->GetHandle());

    if (surfaceFormats.result != vk::Result::eSuccess || surfaceFormats.value.empty())
        return false;

    for (const vk::SurfaceFormatKHR& format: surfaceFormats.value) {
        if (format.format == vk::Format::eB8G8R8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) { // TODO: needs comprehensive selection
            surfaceFormat = *format;
            return true;
        }
    }

    surfaceFormat = *surfaceFormats.value[0];
    return true;
}

vk::Extent2D VulkanSwapChain::GetClampedExtents(uint32_t preferredWidth, uint32_t preferredHeight, const vk::SurfaceCapabilitiesKHR &surfaceProperties) {
    return vk::Extent2D(
        std::clamp(preferredWidth, surfaceProperties.minImageExtent.width, surfaceProperties.maxImageExtent.width),
        std::clamp(preferredHeight, surfaceProperties.minImageExtent.height, surfaceProperties.maxImageExtent.height)
        );
}

vk::PresentModeKHR VulkanSwapChain::GetPresentMode(VulkanDevice *vulkanDevice, VulkanSurface* vulkanSurface, bool blockUntilVBlank) {
    vk::ResultValue<std::vector<vk::PresentModeKHR>> availablePresentModes =
        vulkanDevice->GetPhysicalHandle().getSurfacePresentModesKHR(*vulkanSurface->GetHandle());

    if (availablePresentModes.result != vk::Result::eSuccess)
        return vk::PresentModeKHR::eFifo;

    if (!blockUntilVBlank) {
        for (vk::PresentModeKHR presentMode: availablePresentModes.value) {
            if (presentMode == vk::PresentModeKHR::eMailbox)
                return presentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

VulkanSwapChainState VulkanSwapChain::ResolveSwapChainStateByVulkanResult(vk::Result vkResult) {
    switch (vkResult) {
        case vk::Result::eSuccess:
            return VulkanSwapChainState::Healthy;
        case vk::Result::eErrorOutOfDateKHR:
            return VulkanSwapChainState::OutOtDate;
        case vk::Result::eSuboptimalKHR:
            return VulkanSwapChainState::Suboptimal;
        default:
            return VulkanSwapChainState::Unknown;
    }
}

bool VulkanSwapChain::CanEverRender(VulkanSwapChainState swapChainState) {
    return swapChainState == VulkanSwapChainState::Healthy || swapChainState == VulkanSwapChainState::Suboptimal;
}