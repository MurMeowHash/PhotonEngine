#include "../../Public/Synchronization/VulkanBinarySemaphore.h"
#include "Device/VulkanDevice.h"

VulkanSemaphoreLockResult VulkanBinarySemaphore::ScheduleAcquire() {
    return VulkanSemaphoreLockResult(*m_handle, 0);
}

VulkanSemaphoreLockResult VulkanBinarySemaphore::ScheduleRelease() {
    return VulkanSemaphoreLockResult(*m_handle, 0);
}

VulkanBinarySemaphore* VulkanBinarySemaphore::Create(const VulkanBinarySemaphoreCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::ResultValue<vk::raii::Semaphore> semaphoreWrapper = createInfo.m_vulkanDevice->GetHandle().createSemaphore(vk::SemaphoreCreateInfo());
    if (semaphoreWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanBinarySemaphore* instance = Photon::AllocateObject<VulkanBinarySemaphore>(inOutCreateParams);
    instance->m_handle = std::move(semaphoreWrapper.value);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}
