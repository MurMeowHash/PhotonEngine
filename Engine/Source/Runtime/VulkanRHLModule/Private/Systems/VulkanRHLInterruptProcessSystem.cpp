#include "../../Public/Systems/VulkanRHLInterruptProcessSystem.h"
#include "VulkanDynamicRHL.h"
#include "Configurations/VulkanFrameConfiguration.h"

void VulkanRHLInterruptProcessSystem::Tick() {
    for (VulkanQueue* queue : m_dynamicRHL->GetVulkanDevice()->GetOperatingQueues())
        queue->ProcessInterruptQueue();

    VulkanQueue* gfxQueue;
    if (m_dynamicRHL->GetVulkanDevice()->TryGetQueue(RHLPipeline::Graphics, gfxQueue)
        && gfxQueue->GetWorkBatchesLeft() >= Photon::Vulkan::FrameConfiguration::g_maxAllowedGPUFrameLag) {
            if (gfxQueue->WaitUntilWorkBatchFinished(0) == Photon::Result::Success)
                gfxQueue->ProcessInterruptQueue();

    }
}

void VulkanRHLInterruptProcessSystem::Dispose() { }