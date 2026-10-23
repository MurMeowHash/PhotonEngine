#pragma once

#include "IVulkanDeviceMemoryProviderFactory.h"

namespace Photon::Vulkan::DeviceMemoryProviderFactory {
    IVulkanDeviceMemoryProvider* CreateVulkanDeviceMemoryProvider(VulkanMemoryProviderType memoryProviderType,
        VulkanDevice* vulkanDevice, const VulkanAllocationPresetDefinition& allocationPreset, bool releaseEmptyPages);
}
