#pragma once

#include <vector>
#include "CommandBuffers/VulkanCommandBuffer.h"
#include "Synchronization/VulkanSemaphore.h"

struct VulkanWorkBatch {
    std::vector<VulkanSemaphore*> m_waitSemaphores;
    std::vector<vk::PipelineStageFlags2> m_waitSemaphoresFlags;
    std::vector<VulkanCommandBuffer*> m_commandBuffers;
    std::vector<VulkanSemaphore*> m_signalSemaphores;

    uint64_t m_timelineSemaphoreFinishedValue = 0;
};