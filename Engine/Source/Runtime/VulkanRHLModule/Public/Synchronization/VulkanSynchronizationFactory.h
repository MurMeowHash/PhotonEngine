#pragma once
#include "VulkanBinarySemaphore.h"
#include "VulkanTimelineSemaphore.h"

namespace Photon::Vulkan::SynchronizationFactory {
    VulkanBinarySemaphore* CreateBinarySemaphore(const VulkanBinarySemaphoreCreateInfo& createInfo, bool* isValid);
    VulkanTimelineSemaphore* CreateTimelineSemaphore(const VulkanTimelineSemaphoreCreateInfo& createInfo, bool* isValid);
};