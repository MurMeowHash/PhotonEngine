#pragma once
#include "VulkanCommandBuffer.h"

namespace Photon::Vulkan::CommandBufferFactory {
    VulkanCommandBuffer* CreateCommandBuffer(const VulkanCommandBufferCreateInfo &commandBufferCreateInfo, bool* isValid);
    VulkanCommandBuffer* CreateCommandBuffer(VulkanCommandBufferType commandBufferType, vk::raii::CommandBuffer &&commandBuffer);
}
