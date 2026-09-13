#include "../../Public/CommandBuffers/VulkanCommandBuffer.h"

bool VulkanCommandBuffer::Create(const VulkanCommandBufferCreateInfo &createInfo) {
    vk::CommandBufferAllocateInfo allocateInfo;
    allocateInfo.level = Photon::Vulkan::DeriveLevelFromType(createInfo.m_commandBufferType);
    allocateInfo.commandBufferCount = 1;
    allocateInfo.commandPool = createInfo.m_commandPool;
    vk::ResultValue<std::vector<vk::raii::CommandBuffer>> commandBufferAllocateRes =
        createInfo.m_vulkanDevice->GetHandle().allocateCommandBuffers(allocateInfo);

    if (commandBufferAllocateRes.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(commandBufferAllocateRes.value.front());
    m_commandBufferType = createInfo.m_commandBufferType;

    return true;
}

void VulkanCommandBuffer::Create(VulkanCommandBufferType commandBufferType, vk::raii::CommandBuffer &&commandBuffer) {
    m_handle = std::move(commandBuffer);
    m_commandBufferType = commandBufferType;
}

VulkanCommandBufferType VulkanCommandBuffer::GetCommandBufferType() const {
    return m_commandBufferType;
}

const vk::CommandBuffer& VulkanCommandBuffer::GetHandle() const {
    return *m_handle;
}