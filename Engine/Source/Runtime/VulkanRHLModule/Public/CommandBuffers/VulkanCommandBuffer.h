#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "VulkanCommandBufferGlobals.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Device/VulkanDevice.h"

struct VulkanCommandBufferCreateInfo {
    VulkanCommandBufferType m_commandBufferType;
    const vk::CommandPool& m_commandPool;
    VulkanDevice* m_vulkanDevice;
};

struct VulkanCommandBufferExistingCreateInfo {
    VulkanCommandBufferType m_commandBufferType;
    vk::raii::CommandBuffer&& m_commandBuffer;
};

class VulkanCommandBuffer {
public:
    static VulkanCommandBuffer* Create(const VulkanCommandBufferCreateInfo &createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    static VulkanCommandBuffer* Create(const VulkanCommandBufferExistingCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] VulkanCommandBufferType GetCommandBufferType() const;
    [[nodiscard]] const vk::CommandBuffer& GetHandle() const;

private:
    VulkanCommandBufferType m_commandBufferType = VulkanCommandBufferType::None;
    vk::raii::CommandBuffer m_handle = nullptr;
};
