#pragma once
#include "VulkanCommandBufferPool.h"

namespace Photon::Vulkan::CommandBufferPoolFactory {
    VulkanCommandBufferPool* CreateCommandBufferPool(const VulkanCommandBufferPoolCreateInfo &createInfo, bool* isValid);
}