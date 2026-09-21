#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "VulkanGlobals.h"

namespace Photon::Vulkan::DeviceProvider {
    [[nodiscard]] bool TryFindVulkanDevice(const vk::raii::Instance &vkInstance, uint64_t predefinedDeviceId,
        DeviceSearchFlags flags, vk::raii::PhysicalDevice *physicalDevice);
    [[nodiscard]] uint64_t GetDeviceIdByName(const vk::raii::Instance &vkInstance, const char *physicalDeviceName);
};