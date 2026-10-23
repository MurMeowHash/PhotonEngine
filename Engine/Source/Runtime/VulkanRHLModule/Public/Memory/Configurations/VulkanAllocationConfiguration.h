#pragma once

#include "VulkanAllocationPreset.h"

namespace Photon::Vulkan::AllocationConfiguration {
    inline  VulkanAllocationPresetDefinition g_defaultAllocationPreset = DesktopVulkanAllocationPresetDefinition();
    inline bool g_releaseEmptyMemoryPages = true;
}