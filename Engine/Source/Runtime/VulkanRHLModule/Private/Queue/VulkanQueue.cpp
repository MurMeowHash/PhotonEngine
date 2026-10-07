#include "../../Public/Queue/VulkanQueue.h"
#include "Device/VulkanDevice.h"
#include "CommandBuffers/VulkanCommandBufferPool.h"

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

VulkanQueue* VulkanQueue::Create(const VulkanQueueCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanQueue* instance = Photon::AllocateObject<VulkanQueue>(inOutCreateParams);
    instance->m_handle = createInfo.m_vulkanDevice->GetHandle().getQueue(createInfo.m_queueFamilyIndex, createInfo.m_queueIndex);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    instance->m_queueFamilyIndex = createInfo.m_queueFamilyIndex;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

uint32_t VulkanQueue::GetQueueFamilyIndex() const {
    return m_queueFamilyIndex;
}

VulkanCommandBufferPool* VulkanQueue::AcquireCommandBufferPool(VulkanCommandBufferType commandBufferType,
    VulkanCommandBufferLifetime commandBufferLifetime, Photon::Result& acquireResult) {
    auto commandBufferPoolsIterator = m_commandBufferPools.find(commandBufferType);
    if (commandBufferPoolsIterator != m_commandBufferPools.end()) {
        auto commandPoolIterator = commandBufferPoolsIterator->second.find(commandBufferLifetime);
        if (commandPoolIterator != commandBufferPoolsIterator->second.end() && !commandPoolIterator->second.empty()) {
            VulkanCommandBufferPool* commandBufferPool = commandPoolIterator->second.front();
            commandPoolIterator->second.pop();
            acquireResult = Photon::Result::Success;
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

    InOutCreateParams<Photon::Result> inOutCreateParams{};
    VulkanCommandBufferPool* commandBufferPool = VulkanCommandBufferPool::Create(createInfo, &inOutCreateParams);
    acquireResult = inOutCreateParams.m_result;
    return commandBufferPool;
}

void VulkanQueue::ReturnCommandBufferPool(VulkanCommandBufferPool *commandBufferPool) {
    m_commandBufferPools[commandBufferPool->GetCommandBufferType()][commandBufferPool->GetCommandBufferLifetime()].emplace(commandBufferPool);
}