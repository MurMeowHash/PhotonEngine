#include "../../Public/CommandBuffers/VulkanCommandBuffer.h"

VulkanCommandBuffer * VulkanCommandBuffer::Create(const VulkanCommandBufferCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.level = Photon::Vulkan::DeriveLevelFromType(createInfo.m_commandBufferType);
    allocateInfo.commandBufferCount = 1;
    allocateInfo.commandPool = createInfo.m_commandPool;
    vk::ResultValue<std::vector<vk::raii::CommandBuffer>> commandBufferAllocateRes =
        createInfo.m_vulkanDevice->GetHandle().allocateCommandBuffers(allocateInfo);

    if (commandBufferAllocateRes.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanCommandBuffer* instance = Photon::AllocateObject<VulkanCommandBuffer>(inOutCreateParams);
    instance->m_handle = std::move(commandBufferAllocateRes.value.front());
    instance->m_commandBufferType = createInfo.m_commandBufferType;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanCommandBuffer* VulkanCommandBuffer::Create(const VulkanCommandBufferExistingCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanCommandBuffer* instance = Photon::AllocateObject<VulkanCommandBuffer>(inOutCreateParams);
    instance->m_handle = std::move(createInfo.m_commandBuffer);
    instance->m_commandBufferType = createInfo.m_commandBufferType;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanCommandBufferType VulkanCommandBuffer::GetCommandBufferType() const {
    return m_commandBufferType;
}

const vk::CommandBuffer& VulkanCommandBuffer::GetHandle() const {
    return *m_handle;
}