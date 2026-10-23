#pragma once

#include "IVulkanDeviceMemoryProvider.h"
#include "VulkanMemoryProviderType.h"
#include "Configurations/VulkanAllocationPreset.h"

class VulkanDevice;

class IVulkanDeviceMemoryProviderFactory {
public:
    virtual IVulkanDeviceMemoryProvider* CreateVulkanDeviceMemoryProvider(VulkanMemoryProviderType memoryProviderType,
        VulkanDevice* vulkanDevice, const VulkanAllocationPresetDefinition& allocationPreset, bool releaseEmptyPages) = 0;
    virtual ~IVulkanDeviceMemoryProviderFactory() = default;
};
