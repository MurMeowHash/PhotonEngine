#pragma once

#include "IVulkanDeviceMemoryProvider.h"
#include "VulkanMemoryProviderType.h"
#include "Configurations/VulkanAllocationPreset.h"

class IVulkanDeviceMemoryProviderFactory {
public:
    virtual IVulkanDeviceMemoryProvider* CreateVulkanDeviceMemoryProvider(VulkanMemoryProviderType memoryProviderType,
        vk::raii::Device &device, vk::raii::PhysicalDevice &physicalDevice, const VulkanAllocationPresetDefinition& allocationPreset) = 0;
    virtual ~IVulkanDeviceMemoryProviderFactory() = default;
};
