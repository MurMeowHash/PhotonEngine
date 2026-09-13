#pragma once
#include "VulkanQueue.h"

namespace Photon::Vulkan::QueueFactory {
    VulkanQueue* CreateQueue(const VulkanQueueCreateInfo &createInfo);
}
