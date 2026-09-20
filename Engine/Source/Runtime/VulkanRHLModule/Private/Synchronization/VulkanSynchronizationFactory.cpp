#include "../../Public/Synchronization/VulkanSynchronizationFactory.h"

VulkanBinarySemaphore * Photon::Vulkan::SynchronizationFactory::CreateBinarySemaphore(const VulkanBinarySemaphoreCreateInfo &createInfo, bool *isValid) {
    VulkanBinarySemaphore* semaphore = new VulkanBinarySemaphore();
    bool isCreated = semaphore->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return semaphore;
}

VulkanTimelineSemaphore * Photon::Vulkan::SynchronizationFactory::CreateTimelineSemaphore(const VulkanTimelineSemaphoreCreateInfo &createInfo, bool *isValid) {
    VulkanTimelineSemaphore* semaphore = new VulkanTimelineSemaphore();
    bool isCreated = semaphore->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return semaphore;
}