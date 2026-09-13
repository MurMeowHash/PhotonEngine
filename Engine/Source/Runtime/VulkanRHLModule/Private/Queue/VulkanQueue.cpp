#include "../../Public/Queue/VulkanQueue.h"
#include "Device/VulkanDevice.h"
#include "CommandBuffers/VulkanCommandBufferPool.h"
#include "CommandBuffers/VulkanCommandBufferPoolFactory.h"

VulkanQueue::~VulkanQueue() {
    for (auto& commandBufferPool : m_commandBufferPools) {
        for (auto& pool : commandBufferPool.second) {
            while (!pool.second.empty()) {
                VulkanCommandBufferPool* vulkanCommandPool = pool.second.front();
                delete vulkanCommandPool;
                pool.second.pop();
            }
        }
    }
}

void VulkanQueue::CreateQueue(const VulkanQueueCreateInfo &createInfo) {
    m_handle = createInfo.m_vulkanDevice->GetHandle().getQueue(createInfo.m_queueFamilyIndex, createInfo.m_queueIndex);
    m_vulkanDevice = createInfo.m_vulkanDevice;
    m_queueFamilyIndex = createInfo.m_queueFamilyIndex;
}

uint32_t VulkanQueue::GetQueueFamilyIndex() const {
    return m_queueFamilyIndex;
}

VulkanCommandBufferPool* VulkanQueue::AcquireCommandBufferPool(VulkanCommandBufferType commandBufferType,
    VulkanCommandBufferLifetime commandBufferLifetime) {
    auto commandBufferPoolsIterator = m_commandBufferPools.find(commandBufferType);
    if (commandBufferPoolsIterator != m_commandBufferPools.end()) {
        auto commandPoolIterator = commandBufferPoolsIterator->second.find(commandBufferLifetime);
        if (commandPoolIterator != commandBufferPoolsIterator->second.end() && !commandPoolIterator->second.empty()) {
            VulkanCommandBufferPool* commandBufferPool = commandPoolIterator->second.front();
            commandPoolIterator->second.pop();
            return commandBufferPool;
        }
    }

    VulkanCommandBufferPoolCreateInfo createInfo(
        m_vulkanDevice,
        commandBufferLifetime,
        VulkanCommandBufferCreateFlags::AllowDedicatedReset,
        commandBufferType,
        this,
        0);

    return Photon::Vulkan::CommandBufferPoolFactory::CreateCommandBufferPool(createInfo, nullptr);
}

void VulkanQueue::ReturnCommandBufferPool(VulkanCommandBufferPool *commandBufferPool) {
    m_commandBufferPools[commandBufferPool->GetCommandBufferType()][commandBufferPool->GetCommandBufferLifetime()].emplace(commandBufferPool);
}