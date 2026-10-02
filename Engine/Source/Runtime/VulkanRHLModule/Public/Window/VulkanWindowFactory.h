#pragma once
#include "VulkanSurface.h"

namespace Photon::Vulkan::WindowFactory {
    VulkanSurface* CreateVulkanSurface(const VulkanSurfaceCreateInfo& createInfo, bool* isValid = nullptr);
}