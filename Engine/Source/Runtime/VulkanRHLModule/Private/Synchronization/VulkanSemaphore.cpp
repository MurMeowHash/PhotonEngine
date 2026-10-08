#include "../../Public/Synchronization/VulkanSemaphore.h"

vk::Semaphore VulkanSemaphore::GetHandle() const {
    return *m_handle;
}

vk::SemaphoreSubmitInfo VulkanSemaphore::ScheduleAcquireSubmit(vk::PipelineStageFlags2 pipelineStage) {
    VulkanSemaphoreLockResult acquireResult = ScheduleAcquire();
    return FormSemaphoreSubmitInfo(acquireResult, pipelineStage);
}

vk::SemaphoreSubmitInfo VulkanSemaphore::ScheduleReleaseSubmit() {
    VulkanSemaphoreLockResult releaseResult = ScheduleRelease();
    return FormSemaphoreSubmitInfo(releaseResult, vk::PipelineStageFlagBits2::eAllCommands);
}

vk::SemaphoreSubmitInfo VulkanSemaphore::FormSemaphoreSubmitInfo(const VulkanSemaphoreLockResult &result, vk::PipelineStageFlags2 pipelineStage) {
    vk::SemaphoreSubmitInfo submitInfo;
    submitInfo.semaphore = result.m_semaphoreHandle;
    submitInfo.value = result.m_semaphoreValue;
    submitInfo.stageMask = pipelineStage;
    submitInfo.deviceIndex = 0;
    return submitInfo;
}