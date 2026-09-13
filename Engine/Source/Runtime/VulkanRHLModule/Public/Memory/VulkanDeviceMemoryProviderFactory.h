#pragma once

#include "IVulkanDeviceMemoryProviderFactory.h"

namespace Photon::Vulkan::DeviceMemoryProviderFactory {
    IVulkanDeviceMemoryProvider* CreateVulkanDeviceMemoryProvider(VulkanMemoryProviderType memoryProviderType,
        vk::raii::Device &device, vk::raii::PhysicalDevice &physicalDevice, const VulkanAllocationPresetDefinition& allocationPreset);
}