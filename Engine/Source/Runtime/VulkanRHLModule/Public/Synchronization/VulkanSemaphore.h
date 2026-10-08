#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

struct VulkanSemaphoreLockResult {
    vk::Semaphore m_semaphoreHandle;
    uint64_t m_semaphoreValue;
};

class VulkanSemaphore {
public:
    virtual ~VulkanSemaphore() = default;
    [[nodiscard]] vk::Semaphore GetHandle() const;
    [[nodiscard]] virtual VulkanSemaphoreLockResult ScheduleAcquire() = 0;
    [[nodiscard]] virtual VulkanSemaphoreLockResult ScheduleRelease() = 0;
    [[nodiscard]] vk::SemaphoreSubmitInfo ScheduleAcquireSubmit(vk::PipelineStageFlags2 pipelineStage);
    [[nodiscard]] vk::SemaphoreSubmitInfo ScheduleReleaseSubmit();
protected:
    vk::raii::Semaphore m_handle = nullptr;

    [[nodiscard]] static vk::SemaphoreSubmitInfo FormSemaphoreSubmitInfo(const VulkanSemaphoreLockResult& result, vk::PipelineStageFlags2 pipelineStage);
};