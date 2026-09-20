#include "../../Public/Synchronization/VulkanTimelineSemaphore.h"
#include "Device/VulkanDevice.h"

bool VulkanTimelineSemaphore::Create(const VulkanTimelineSemaphoreCreateInfo &createInfo) {
    vk::SemaphoreTypeCreateInfo semaphoreTypeCreateInfo;
    semaphoreTypeCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
    semaphoreTypeCreateInfo.initialValue = createInfo.m_initialValue;

    vk::StructureChain<vk::SemaphoreCreateInfo, vk::SemaphoreTypeCreateInfo> semaphoreStructureChain = {
        vk::SemaphoreCreateInfo(),
        semaphoreTypeCreateInfo
    };

    vk::ResultValue<vk::raii::Semaphore> semaphoreWrapper = createInfo.m_vulkanDevice->GetHandle().createSemaphore(
        semaphoreStructureChain.get<vk::SemaphoreCreateInfo>());

    if (semaphoreWrapper.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(semaphoreWrapper.value);
    m_vulkanDevice = createInfo.m_vulkanDevice;
    return true;
}

bool VulkanTimelineSemaphore::WaitForValue(uint64_t waitValue, uint64_t timeout) const {
    vk::SemaphoreWaitInfo waitInfo;
    waitInfo.pValues = &waitValue;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &*m_handle;

    vk::Result waitResult = m_vulkanDevice->GetHandle().waitSemaphores(waitInfo, timeout);
    return waitResult == vk::Result::eSuccess;
}

bool VulkanTimelineSemaphore::SignalValue(uint64_t signalValue) const {
    vk::SemaphoreSignalInfo signalInfo;
    signalInfo.value = signalValue;
    signalInfo.semaphore = *m_handle;

    vk::Result signalResult = m_vulkanDevice->GetHandle().signalSemaphore(signalInfo);
    return signalResult == vk::Result::eSuccess;
}

bool VulkanTimelineSemaphore::TryGetCurrentValue(uint64_t &value) const {
    vk::ResultValue<uint64_t> valueGetWrapper = m_handle.getCounterValue();
    if (valueGetWrapper.result != vk::Result::eSuccess)
        return false;

    value = valueGetWrapper.value;
    return true;
}