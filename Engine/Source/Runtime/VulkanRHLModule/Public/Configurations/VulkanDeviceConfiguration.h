#pragma once

#include <vector>

#include "../Device/VulkanDeviceFeature.h"

namespace Photon::Vulkan::DeviceConfiguration {
    inline std::vector<VulkanDeviceFeature> g_deviceFeatures = {
        VulkanDeviceFeature(VulkanDeviceFeatureType::ShaderDrawParameters, true),
        VulkanDeviceFeature(VulkanDeviceFeatureType::Synchronization2, true),
        VulkanDeviceFeature(VulkanDeviceFeatureType::DynamicRendering, true),
        VulkanDeviceFeature(VulkanDeviceFeatureType::TimelineSemaphore, true)
    };
};