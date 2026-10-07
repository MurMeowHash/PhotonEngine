#pragma once
#include <queue>
#define VULKAN_HPP_NO_EXCEPTIONS
#include <cstdint>
#include <vulkan/vulkan_raii.hpp>
#include "VulkanCommandBuffer.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Device/VulkanDevice.h"
#include "VulkanCommandBufferGlobals.h"

enum class VulkanCommandBufferCreateFlags : uint32_t {
    None = 0,
    AllowDedicatedReset = 1 << 0,
};

class VulkanQueue;

struct VulkanCommandBufferPoolCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanCommandBufferLifetime m_commandBufferLifetime;
    VulkanCommandBufferCreateFlags m_createFlags;
    VulkanCommandBufferType m_commandBufferType;
    VulkanQueue* m_vulkanQueue;
    uint32_t m_initialPoolSize;
};

class VulkanCommandBufferPool {
public:
    ~VulkanCommandBufferPool();

    static VulkanCommandBufferPool* Create(const VulkanCommandBufferPoolCreateInfo &createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] const vk::CommandPool& GetHandle() const;
    [[nodiscard]] VulkanCommandBufferType GetCommandBufferType() const;
    [[nodiscard]] VulkanCommandBufferLifetime GetCommandBufferLifetime() const;
    [[nodiscard]] VulkanCommandBuffer* PopCommandBuffer(Photon::Result& popResult);
    void ReturnCommandBuffer(VulkanCommandBuffer *commandBuffer);

private:
    vk::raii::CommandPool m_handle = nullptr;
    VulkanCommandBufferType m_commandBufferType = VulkanCommandBufferType::None;
    VulkanCommandBufferLifetime m_commandBufferLifetime = VulkanCommandBufferLifetime::None;
    VulkanDevice* m_poolDevice = nullptr;
    std::queue<VulkanCommandBuffer*> m_commandBufferPool;

    [[nodiscard]] Photon::Result TryExtendPool();
    [[nodiscard]] Photon::Result PopulatePool(uint32_t poolSize);
};
