#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

class VulkanDevice;
class VulkanSurface;

struct VulkanSwapChainCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanSurface* m_vulkanSurface;
    uint32_t m_desiredImageCount;
};

class VulkanSwapChain {
public:
    [[nodiscard]] bool Create(const VulkanSwapChainCreateInfo& createInfo);
private:
    [[nodiscard]] static uint32_t GetSwapChainMinImageCount(const vk::SurfaceCapabilitiesKHR& surfaceProperties);
    [[nodiscard]] static bool TryGetSurfaceFormat(VulkanDevice* vulkanDevice, VulkanSurface* vulkanSurface, VkSurfaceFormatKHR& surfaceFormat);
};