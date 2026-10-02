#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include <windows.h>

class VulkanInstance;

struct VulkanSurfaceCreateInfo {
    VulkanInstance* m_vulkanInstance;
    HWND m_windowHWND;
};

class VulkanSurface {
public:
    [[nodiscard]] bool Create(const VulkanSurfaceCreateInfo& createInfo);
    [[nodiscard]] const vk::raii::SurfaceKHR& GetHandle() const;
private:
    vk::raii::SurfaceKHR m_handle = nullptr;
};