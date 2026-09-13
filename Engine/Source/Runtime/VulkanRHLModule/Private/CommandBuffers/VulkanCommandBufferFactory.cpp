#include "../../Public/CommandBuffers/VulkanCommandBufferFactory.h"

VulkanCommandBuffer * Photon::Vulkan::CommandBufferFactory::CreateCommandBuffer(const VulkanCommandBufferCreateInfo &commandBufferCreateInfo, bool* isValid) {
    VulkanCommandBuffer* commandBuffer = new VulkanCommandBuffer();
    bool created = commandBuffer->Create(commandBufferCreateInfo);
    if (isValid)
        *isValid = created;

    return commandBuffer;
}

VulkanCommandBuffer * Photon::Vulkan::CommandBufferFactory::CreateCommandBuffer(VulkanCommandBufferType commandBufferType, vk::raii::CommandBuffer &&commandBuffer) {
    VulkanCommandBuffer* vulkanCommandBuffer = new VulkanCommandBuffer();
    vulkanCommandBuffer->Create(commandBufferType, std::move(commandBuffer));
    return vulkanCommandBuffer;
}