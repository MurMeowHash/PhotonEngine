#include "../Public/VulkanRHLCommandWorkContext.h"
#include "VulkanDynamicRHL.h"
#include "VulkanUtils.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

Photon::Result VulkanRHLCommandWorkContext::InitializeContext(RHLPipeline pipeline) {
    if (g_vulkanDynamicRHL == nullptr)
        return Photon::Result::UnknownFailure;

    VulkanQueue* queue;
    if (!g_vulkanDynamicRHL->GetVulkanDevice()->TryGetQueue(pipeline, queue))
        return Photon::Result::UnknownFailure;

    m_contextPipeline = pipeline;
    Initialize(queue, g_vulkanDynamicRHL->GetVulkanDevice());
    return Photon::Result::Success;
}

RHLPipeline VulkanRHLCommandWorkContext::GetRHLPipeline() const {
    return m_contextPipeline;
}

Photon::Result VulkanRHLCommandWorkContext::Execute() {
    VulkanWorkSubmitInfo submitInfo;
    Photon::Result packResult = PackWorkBatches(submitInfo);
    if (packResult != Photon::Result::Success)
        return packResult;

    return m_workQueue->SubmitWorkBatches(std::move(submitInfo));
}

void VulkanRHLCommandWorkContext::BeginTransition(const VulkanRHLPendingTransition& pendingTransition) {
    if (!pendingTransition.IsCrossPipeline())
        return;

    ProcessTransition(pendingTransition, false, true);
    AddSignalSemaphore(pendingTransition.GetTransitionSemaphore());
}

void VulkanRHLCommandWorkContext::EndTransition(const VulkanRHLPendingTransition &pendingTransition) {
    if (!pendingTransition.IsCrossPipeline())
        return;

    ProcessTransition(pendingTransition, true, false);
    AddWaitSemaphore(pendingTransition.GetTransitionSemaphore(), vk::PipelineStageFlagBits2::eAllCommands);
}

void VulkanRHLCommandWorkContext::ExecMonoTransition(const VulkanRHLPendingTransition &pendingTransition) {
    if (pendingTransition.IsCrossPipeline())
        return;

    ProcessTransition(pendingTransition, false, false);
}

VulkanPipelineImageTransitionScope VulkanRHLCommandWorkContext::CreatePipelineImageTransitionScope(const VulkanImageTransitionScope &scope,
                                                                                                   uint32_t queueFamily, bool ignoreStageAccess) {
    VulkanPipelineImageTransitionScope pipelineScope{};
    pipelineScope.m_stage = ignoreStageAccess ? vk::PipelineStageFlagBits2::eNone : scope.m_stage;
    pipelineScope.m_access = ignoreStageAccess ? vk::AccessFlagBits2::eNone : scope.m_access;
    pipelineScope.m_layout = scope.m_layout;
    pipelineScope.m_queueFamily = queueFamily;
    return pipelineScope;
}

void VulkanRHLCommandWorkContext::ProcessTransition(const VulkanRHLPendingTransition &pendingTransition, bool ignoreSrcStageAccess, bool ignoreDestStageAccess) {
    VulkanPipelineBarrier barrier;
    for (const VulkanImagePendingTransition& imagePendingTransition : pendingTransition.GetImagePendingTransitions()) {
        VulkanPipelineImageTransitionScope srcPipelineScope = CreatePipelineImageTransitionScope(imagePendingTransition.m_srcScope,
            pendingTransition.GetSrcQueueFamily(), ignoreSrcStageAccess);
        VulkanPipelineImageTransitionScope destPipelineScope = CreatePipelineImageTransitionScope(imagePendingTransition.m_destScope,
            pendingTransition.GetDestQueueFamily(), ignoreDestStageAccess);

        barrier.TransitionImage(imagePendingTransition.m_rhlTexture, srcPipelineScope, destPipelineScope, imagePendingTransition.m_subresourceRange);
    }

    VulkanCommandBuffer* cmdBuffer = GetCommandBuffer();
    cmdBuffer->ExecutePipelineBarrier(std::move(barrier));
}