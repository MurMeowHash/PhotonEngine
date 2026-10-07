#pragma once
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include <map>
#include <queue>
#include <cstdint>
#include "CommandBuffers/VulkanCommandBufferGlobals.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

class VulkanDevice;
class VulkanCommandBufferPool;

struct VulkanQueueCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint32_t m_queueFamilyIndex;
    uint32_t m_queueIndex;
};

class VulkanQueue {
public:
    ~VulkanQueue();
    static VulkanQueue* Create(const VulkanQueueCreateInfo &createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] uint32_t GetQueueFamilyIndex() const;
    [[nodiscard]] VulkanCommandBufferPool* AcquireCommandBufferPool(VulkanCommandBufferType commandBufferType,
        VulkanCommandBufferLifetime commandBufferLifetime, Photon::Result& acquireResult);
    void ReturnCommandBufferPool(VulkanCommandBufferPool *commandBufferPool);
private:
    vk::raii::Queue m_handle = nullptr;
    uint32_t m_queueFamilyIndex = 0;
    VulkanDevice* m_vulkanDevice = nullptr;

    std::map<VulkanCommandBufferType, std::map<VulkanCommandBufferLifetime, std::queue<VulkanCommandBufferPool*>>> m_commandBufferPools;
};