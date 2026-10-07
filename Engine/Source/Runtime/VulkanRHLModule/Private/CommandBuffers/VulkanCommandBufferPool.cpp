#include "../../Public/CommandBuffers/VulkanCommandBufferPool.h"
#include "CoreUtils.h"
#include "Queue/VulkanQueue.h"

VulkanCommandBufferPool::~VulkanCommandBufferPool() {
    while (!m_commandBufferPool.empty()) {
        VulkanCommandBuffer* commandBuffer = m_commandBufferPool.front();
        delete commandBuffer;
        m_commandBufferPool.pop();
    }
}

VulkanCommandBufferPool* VulkanCommandBufferPool::Create(const VulkanCommandBufferPoolCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::CommandPoolCreateInfo poolCreateInfo{};
    if (Photon::Core::Flags::IsFlagSet(VulkanCommandBufferCreateFlags::AllowDedicatedReset, createInfo.m_createFlags))
        poolCreateInfo.flags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if (createInfo.m_commandBufferLifetime == VulkanCommandBufferLifetime::ShortLived)
        poolCreateInfo.flags |= vk::CommandPoolCreateFlagBits::eTransient;

    poolCreateInfo.queueFamilyIndex = createInfo.m_vulkanQueue->GetQueueFamilyIndex();

    vk::ResultValue<vk::raii::CommandPool> poolCreateResult =
        createInfo.m_vulkanDevice->GetHandle().createCommandPool(poolCreateInfo);

    if (poolCreateResult.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanCommandBufferPool* instance = Photon::AllocateObject<VulkanCommandBufferPool>(inOutCreateParams);
    instance->m_handle = std::move(poolCreateResult.value);
    instance->m_poolDevice = createInfo.m_vulkanDevice;
    instance->m_commandBufferType = createInfo.m_commandBufferType;
    instance->m_commandBufferLifetime = createInfo.m_commandBufferLifetime;
    Photon::Result poolPopulateResult = instance->PopulatePool(createInfo.m_initialPoolSize);
    Photon::PushResult(poolPopulateResult, inOutCreateParams);
    return instance;
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

VulkanCommandBuffer* VulkanCommandBufferPool::PopCommandBuffer(Photon::Result& popResult) {
    if (m_commandBufferPool.empty()) {
        Photon::Result extendResult = TryExtendPool();
        if (extendResult != Photon::Result::Success) {
            popResult = extendResult;
            return nullptr;
        }
    }

    VulkanCommandBuffer* pooledBuffer = m_commandBufferPool.front();
    m_commandBufferPool.pop();
    popResult = Photon::Result::Success;
    return pooledBuffer;
}

void VulkanCommandBufferPool::ReturnCommandBuffer(VulkanCommandBuffer *commandBuffer) {
    m_commandBufferPool.emplace(commandBuffer);
}

Photon::Result VulkanCommandBufferPool::TryExtendPool() {
    VulkanCommandBufferCreateInfo createInfo(m_commandBufferType, *m_handle, m_poolDevice);
    InOutCreateParams<Photon::Result> inOutCreateParams{};
    VulkanCommandBuffer* commandBuffer = VulkanCommandBuffer::Create(createInfo, &inOutCreateParams);
    if (inOutCreateParams.m_result != Photon::Result::Success)
        return inOutCreateParams.m_result;

    m_commandBufferPool.emplace(commandBuffer);
    return Photon::Result::Success;
}

Photon::Result VulkanCommandBufferPool::PopulatePool(uint32_t poolSize) {
    if (poolSize == 0)
        return Photon::Result::Success;

    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.level = Photon::Vulkan::DeriveLevelFromType(m_commandBufferType);
    allocateInfo.commandBufferCount = poolSize;
    allocateInfo.commandPool = m_handle;

    vk::ResultValue<std::vector<vk::raii::CommandBuffer>> bufferAllocatedResult =
        m_poolDevice->GetHandle().allocateCommandBuffers(allocateInfo);

    if (bufferAllocatedResult.result != vk::Result::eSuccess)
        return Photon::Result::UnknownFailure;

    for (vk::raii::CommandBuffer& commandBuffer : bufferAllocatedResult.value) {
        VulkanCommandBufferExistingCreateInfo createInfo{m_commandBufferType, std::move(commandBuffer)};
        InOutCreateParams<Photon::Result> inOutCreateParams{};
        VulkanCommandBuffer* vkCmdBuffer = VulkanCommandBuffer::Create(createInfo, &inOutCreateParams);
        if (inOutCreateParams.m_result != Photon::Result::Success)
            return inOutCreateParams.m_result;

        m_commandBufferPool.emplace(vkCmdBuffer);
    }

    return Photon::Result::Success;
}