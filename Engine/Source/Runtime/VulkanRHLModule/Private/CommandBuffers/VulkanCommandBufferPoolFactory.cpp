#include "../../Public/CommandBuffers/VulkanCommandBufferPoolFactory.h"

VulkanCommandBufferPool* Photon::Vulkan::CommandBufferPoolFactory::CreateCommandBufferPool(const VulkanCommandBufferPoolCreateInfo &createInfo, bool* isValid) {
    VulkanCommandBufferPool* vulkanCommandBufferPool = new VulkanCommandBufferPool();
    bool created = vulkanCommandBufferPool->Create(createInfo);
    if (isValid)
        *isValid = created;

    return vulkanCommandBufferPool;
}