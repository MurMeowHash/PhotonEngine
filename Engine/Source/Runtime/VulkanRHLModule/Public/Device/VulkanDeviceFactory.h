#pragma once
#include "VulkanDevice.h"

namespace Photon::Vulkan::DeviceFactory {
    VulkanDevice* CreateVulkanDevice(const VulkanDeviceCreateInfo& createInfo, bool* isValid);
};