#include "../../Public/Synchronization/VulkanTimelineSemaphore.h"
#include "Device/VulkanDevice.h"

VulkanSemaphoreLockResult VulkanTimelineSemaphore::ScheduleAcquire() {
    return VulkanSemaphoreLockResult(*m_handle, m_timelineValue);
}

VulkanSemaphoreLockResult VulkanTimelineSemaphore::ScheduleRelease() {
    ++m_timelineValue;
    return VulkanSemaphoreLockResult(*m_handle, m_timelineValue);
}

VulkanTimelineSemaphore * VulkanTimelineSemaphore::Create(const VulkanTimelineSemaphoreCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::SemaphoreTypeCreateInfo semaphoreTypeCreateInfo;
    semaphoreTypeCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
    semaphoreTypeCreateInfo.initialValue = createInfo.m_initialValue;

    vk::StructureChain<vk::SemaphoreCreateInfo, vk::SemaphoreTypeCreateInfo> semaphoreStructureChain = {
        vk::SemaphoreCreateInfo(),
        semaphoreTypeCreateInfo
    };

    vk::ResultValue<vk::raii::Semaphore> semaphoreWrapper = createInfo.m_vulkanDevice->GetHandle().createSemaphore(
        semaphoreStructureChain.get<vk::SemaphoreCreateInfo>());

    if (semaphoreWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanTimelineSemaphore* instance = Photon::AllocateObject<VulkanTimelineSemaphore>(inOutCreateParams);
    instance->m_handle = std::move(semaphoreWrapper.value);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    instance->m_timelineValue = createInfo.m_initialValue;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

uint64_t VulkanTimelineSemaphore::GetCurrentScheduledValue() const {
    return m_timelineValue;
}

Photon::Result VulkanTimelineSemaphore::TryGetCurrentTimelineValue(uint64_t& value) const {
    vk::ResultValue<uint64_t> valueWrapper = m_handle.getCounterValue();
    if (valueWrapper.result != vk::Result::eSuccess)
        return Photon::Result::UnknownFailure;

    value = valueWrapper.value;
    return Photon::Result::Success;
}