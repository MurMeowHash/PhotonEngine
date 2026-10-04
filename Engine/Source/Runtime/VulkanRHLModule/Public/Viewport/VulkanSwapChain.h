#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Resources/VulkanRHLTexture.h"

class VulkanDevice;
class VulkanSurface;
class VulkanSwapChain;

struct VulkanSwapChainCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanSurface* m_vulkanSurface;
    uint32_t m_width;
    uint32_t m_height;
    bool m_blockUntilVBlank;
    VulkanSwapChain* m_oldSwapChain;
};

class VulkanSwapChain {
public:
    [[nodiscard]] static VulkanSwapChain* Create(const VulkanSwapChainCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] const vk::raii::SwapchainKHR& GetHandle() const;
private:
    vk::raii::SwapchainKHR m_handle = nullptr;
    std::vector<VulkanRHLTexture*> m_images;
    vk::Format m_format{};

    VulkanDevice* m_vulkanDevice = nullptr;

    [[nodiscard]] Photon::Result RetrieveImages(uint32_t arrayLayers);

    [[nodiscard]] static uint32_t GetSwapChainMinImageCount(const vk::SurfaceCapabilitiesKHR& surfaceProperties);
    [[nodiscard]] static bool TryGetSurfaceFormat(VulkanDevice* vulkanDevice, VulkanSurface* vulkanSurface, VkSurfaceFormatKHR& surfaceFormat);
    [[nodiscard]] static vk::Extent2D GetClampedExtents(uint32_t preferredWidth, uint32_t preferredHeight, const vk::SurfaceCapabilitiesKHR& surfaceProperties);
    [[nodiscard]] static vk::PresentModeKHR GetPresentMode(VulkanDevice* vulkanDevice, VulkanSurface* vulkanSurface, bool vSyncEnabled);
};