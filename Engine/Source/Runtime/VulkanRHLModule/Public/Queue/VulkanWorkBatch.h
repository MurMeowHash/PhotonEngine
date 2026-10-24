#pragma once

#include <vector>
#include "CommandBuffers/VulkanCommandBuffer.h"
#include "Synchronization/VulkanSemaphore.h"
#include "VulkanWorkSyncEvent.h"

enum class VulkanWorkBatchStatus {
    None = 0,
    Recorded = 1,
    Packed = 2,
    Finished = 3,
};

struct VulkanWorkBatch {
    std::vector<std::shared_ptr<VulkanSemaphore>> m_waitSemaphores;
    std::vector<vk::PipelineStageFlags2> m_waitSemaphoresFlags;
    std::vector<VulkanCommandBuffer*> m_commandBuffers;
    std::vector<std::shared_ptr<VulkanSemaphore>> m_signalSemaphores;
    std::vector<VulkanWorkSyncEvent> m_syncEvents;

    uint64_t m_timelineSemaphoreFinishedValue = 0;
    VulkanWorkBatchStatus m_batchStatus = VulkanWorkBatchStatus::None;
};