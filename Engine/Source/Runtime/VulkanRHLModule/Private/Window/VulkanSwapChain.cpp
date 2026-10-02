#include "../../Public/Window/VulkanSwapChain.h"
#include "Device/VulkanDevice.h"
#include "Window/VulkanSurface.h"

bool VulkanSwapChain::Create(const VulkanSwapChainCreateInfo &createInfo) {
    vk::ResultValue<vk::SurfaceCapabilitiesKHR> surfaceProperties =
        createInfo.m_vulkanDevice->GetPhysicalHandle().getSurfaceCapabilitiesKHR(*createInfo.m_vulkanSurface->GetHandle());

    vk::ResultValue<std::vector<vk::SurfaceFormatKHR>> surfaceFormats =
        createInfo.m_vulkanDevice->GetPhysicalHandle().getSurfaceFormatsKHR(*createInfo.m_vulkanSurface->GetHandle());

    if (surfaceProperties.result != vk::Result::eSuccess || surfaceFormats.result != vk::Result::eSuccess)
        return false;

    vk::SwapchainCreateInfoKHR swapChainCreateInfo;
    swapChainCreateInfo.surface = *createInfo.m_vulkanSurface->GetHandle();
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
        if (format.format == vk::Format::eB8G8R8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            surfaceFormat = *format;
            return true;
        }
    }

    surfaceFormat = *surfaceFormats.value[0];
    return true;
}