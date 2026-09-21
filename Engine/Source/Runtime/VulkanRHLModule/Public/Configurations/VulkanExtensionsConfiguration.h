#pragma once

#include <vector>
#include <windows.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>
#include "Entity/VulkanExtension.h"

namespace Photon::Vulkan::ExtensionsConfiguration {
    inline std::vector<VulkanExtension> g_instanceExtensions = {
        VulkanExtension(vk::KHRSurfaceExtensionName, true),
        VulkanExtension(vk::EXTDebugUtilsExtensionName, true),
        VulkanExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true)
    };

    inline std::vector<VulkanExtension> g_deviceExtensions = {
        VulkanExtension(vk::KHRSwapchainExtensionName, true),
        VulkanExtension(vk::KHRSpirv14ExtensionName, true),
        VulkanExtension(vk::KHRSynchronization2ExtensionName, true),
        VulkanExtension(vk::KHRDynamicRenderingExtensionName, true)
    };
}