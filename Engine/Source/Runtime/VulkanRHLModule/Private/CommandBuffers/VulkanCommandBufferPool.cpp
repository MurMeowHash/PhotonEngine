#include "../../Public/CommandBuffers/VulkanCommandBufferPool.h"
#include "CoreUtils.h"
#include "CommandBuffers/VulkanCommandBufferFactory.h"
#include "Queue/VulkanQueue.h"

VulkanCommandBufferPool::~VulkanCommandBufferPool() {
    while (!m_commandBufferPool.empty()) {
        VulkanCommandBuffer* commandBuffer = m_commandBufferPool.front();
        delete commandBuffer;
        m_commandBufferPool.pop();
    }
}

bool VulkanCommandBufferPool::Create(const VulkanCommandBufferPoolCreateInfo &createInfo) {
    vk::CommandPoolCreateInfo poolCreateInfo;
    if (Photon::Core::Flags::IsFlagSet(VulkanCommandBufferCreateFlags::AllowDedicatedReset, createInfo.m_createFlags))
        poolCreateInfo.flags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if (createInfo.m_commandBufferLifetime == VulkanCommandBufferLifetime::ShortLived)
        poolCreateInfo.flags |= vk::CommandPoolCreateFlagBits::eTransient;

    poolCreateInfo.queueFamilyIndex = createInfo.m_vulkanQueue->GetQueueFamilyIndex();

    vk::ResultValue<vk::raii::CommandPool> poolCreateResult =
        createInfo.m_vulkanDevice->GetHandle().createCommandPool(poolCreateInfo);

    if (poolCreateResult.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(poolCreateResult.value);
    m_poolDevice = createInfo.m_vulkanDevice;
    m_commandBufferType = createInfo.m_commandBufferType;
    m_commandBufferLifetime = createInfo.m_commandBufferLifetime;

    PopulatePool(createInfo.m_initialPoolSize);
    return true;
}

const vk::CommandPool& VulkanCommandBufferPool::GetHandle() const {
    return *m_handle;
}

VulkanCommandBufferType VulkanCommandBufferPool::GetCommandBufferType() const {
    return m_commandBufferType;
}

VulkanCommandBufferLifetime VulkanCommandBufferPool::GetCommandBufferLifetime() const {
    return m_commandBufferLifetime;
}

VulkanCommandBuffer* VulkanCommandBufferPool::PopCommandBuffer() {
    if (m_commandBufferPool.empty())
        if (!TryExtendPool())
            return nullptr;

    VulkanCommandBuffer* pooledBuffer = m_commandBufferPool.front();
    m_commandBufferPool.pop();
    return pooledBuffer;
}

void VulkanCommandBufferPool::ReturnCommandBuffer(VulkanCommandBuffer *commandBuffer) {
    m_commandBufferPool.emplace(commandBuffer);
}

bool VulkanCommandBufferPool::TryExtendPool() {
    VulkanCommandBufferCreateInfo createInfo(m_commandBufferType, *m_handle, m_poolDevice);
    bool isValid;
    VulkanCommandBuffer* commandBuffer = Photon::Vulkan::CommandBufferFactory::CreateCommandBuffer(createInfo, &isValid);
    if (isValid)
        m_commandBufferPool.emplace(commandBuffer);
    else
        delete commandBuffer;

    return isValid;
}

void VulkanCommandBufferPool::PopulatePool(uint32_t poolSize) {
    if (poolSize == 0)
        return;

    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.level = Photon::Vulkan::DeriveLevelFromType(m_commandBufferType);
    allocateInfo.commandBufferCount = poolSize;
    allocateInfo.commandPool = m_handle;

    vk::ResultValue<std::vector<vk::raii::CommandBuffer>> bufferAllocatedResult =
        m_poolDevice->GetHandle().allocateCommandBuffers(allocateInfo);

    if (bufferAllocatedResult.result != vk::Result::eSuccess)
        return;

    for (vk::raii::CommandBuffer& commandBuffer : bufferAllocatedResult.value) {
        m_commandBufferPool.emplace(Photon::Vulkan::CommandBufferFactory::CreateCommandBuffer(m_commandBufferType, std::move(commandBuffer)));
    }
}