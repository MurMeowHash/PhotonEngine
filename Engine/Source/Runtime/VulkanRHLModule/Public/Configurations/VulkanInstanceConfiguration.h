#pragma once

#include <vector>

namespace Photon::Vulkan::VulkanInstanceConfiguration {
    inline std::vector<const char*> g_validationLayers = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor"
    };
}