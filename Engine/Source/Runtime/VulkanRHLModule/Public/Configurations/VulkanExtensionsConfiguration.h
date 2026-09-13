#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_win32.h>

namespace Photon::Vulkan::VulkanExtensionsConfiguration {
    inline std::vector<const char*> g_instanceExtensions = {
        vk::KHRSurfaceExtensionName,
        vk::EXTDebugUtilsExtensionName,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
}