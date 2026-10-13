#include "../../Public/Synchronization/VulkanSemaphorePool.h"

VulkanBinarySemaphore* VulkanBinarySemaphorePool::CreateObject(InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanBinarySemaphoreCreateInfo createInfo{};
    createInfo.m_vulkanDevice = m_vulkanDevice;
    return VulkanBinarySemaphore::Create(createInfo, inOutCreateParams);
}

VulkanBinarySemaphorePool* VulkanBinarySemaphorePool::Create(const VulkanBinarySemaphorePoolCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanBinarySemaphorePool* instance = Photon::AllocateObject<VulkanBinarySemaphorePool>(inOutCreateParams);
    instance->Initialize(0, UINT32_MAX);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}