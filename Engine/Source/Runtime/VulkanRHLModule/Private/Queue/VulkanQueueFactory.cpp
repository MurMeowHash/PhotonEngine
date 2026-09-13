#include "../../Public/Queue/VulkanQueueFactory.h"

VulkanQueue* Photon::Vulkan::QueueFactory::CreateQueue(const VulkanQueueCreateInfo &createInfo) {
    VulkanQueue* queue = new VulkanQueue();
    queue->CreateQueue(createInfo);
    return queue;
}
