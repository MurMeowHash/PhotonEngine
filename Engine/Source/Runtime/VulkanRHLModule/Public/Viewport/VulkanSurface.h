#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include <windows.h>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

class VulkanInstance;

struct VulkanSurfaceCreateInfo {
    VulkanInstance* m_vulkanInstance;
    HWND m_windowHWND;
};

class VulkanSurface {
public:
    static VulkanSurface* Create(const VulkanSurfaceCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] const vk::raii::SurfaceKHR& GetHandle() const;
private:
    vk::raii::SurfaceKHR m_handle = nullptr;
};