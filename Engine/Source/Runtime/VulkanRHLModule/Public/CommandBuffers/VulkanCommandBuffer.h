#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "VulkanCommandBufferGlobals.h"
#include "../Device/VulkanDevice.h"

struct VulkanCommandBufferCreateInfo {
    VulkanCommandBufferType m_commandBufferType;
    const vk::CommandPool& m_commandPool;
    VulkanDevice* m_vulkanDevice;
};

class VulkanCommandBuffer {
public:
    [[nodiscard]] bool Create(const VulkanCommandBufferCreateInfo &createInfo);
    void Create(VulkanCommandBufferType commandBufferType, vk::raii::CommandBuffer &&commandBuffer);
    [[nodiscard]] VulkanCommandBufferType GetCommandBufferType() const;
    [[nodiscard]] const vk::CommandBuffer& GetHandle() const;

private:
    VulkanCommandBufferType m_commandBufferType = VulkanCommandBufferType::None;
    vk::raii::CommandBuffer m_handle = nullptr;
};
