#pragma once
#include "VulkanDynamicRHL.h"

namespace Photon::Vulkan::RHLFactory {
    VulkanDynamicRHL* CreateVulkanDynamicRHL(bool* isValid);
};