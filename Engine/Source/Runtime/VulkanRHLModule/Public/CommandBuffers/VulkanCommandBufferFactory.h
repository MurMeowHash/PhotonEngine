#pragma once
#include "VulkanCommandBuffer.h"

namespace Photon::Vulkan::CommandBufferFactory {
    VulkanCommandBuffer* CreateCommandBuffer(const VulkanCommandBufferCreateInfo &commandBufferCreateInfo, bool* isValid = nullptr);
    VulkanCommandBuffer* CreateCommandBuffer(VulkanCommandBufferType commandBufferType, vk::raii::CommandBuffer &&commandBuffer);
}
