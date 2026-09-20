#include "../../Public/Synchronization/VulkanBinarySemaphore.h"
#include "Device/VulkanDevice.h"

bool VulkanBinarySemaphore::Create(const VulkanBinarySemaphoreCreateInfo& createInfo) {
    vk::ResultValue<vk::raii::Semaphore> semaphoreWrapper = createInfo.m_vulkanDevice->GetHandle().createSemaphore(vk::SemaphoreCreateInfo());
    if (semaphoreWrapper.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(semaphoreWrapper.value);
    return true;
}