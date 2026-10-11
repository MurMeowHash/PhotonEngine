#include "../Public/VulkanRHLCommandWorkContext.h"
#include "VulkanDynamicRHL.h"
#include "VulkanUtils.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

Photon::Result VulkanRHLCommandWorkContext::InitializeContext() {
    if (g_vulkanDynamicRHL == nullptr)
        return Photon::Result::UnknownFailure;

    VulkanQueue* queue;
    if (!g_vulkanDynamicRHL->GetVulkanDevice()->TryGetQueue(GetQueueTypeByRHLWork(GetVulkanRHLWorkType()), queue))
        return Photon::Result::UnknownFailure;

    Initialize(queue, g_vulkanDynamicRHL->GetVulkanDevice());
    return Photon::Result::Success;
}

Photon::Result VulkanRHLCommandWorkContext::FinalizeWork() {
    VulkanWorkSubmitInfo submitInfo;
    Photon::Result packResult = PackWorkBatches(submitInfo);
    if (packResult != Photon::Result::Success)
        return packResult;

    return m_workQueue->SubmitWorkBatches(std::move(submitInfo));
}

void VulkanRHLCommandWorkContext::BeginTransition(const VulkanRHLPendingTransition& pendingTransition) {
    ProcessTransition(pendingTransition, false, true);
    if (pendingTransition.IsCrossQueue())
        AddSignalSemaphore(pendingTransition.GetTransitionSemaphore());
}

void VulkanRHLCommandWorkContext::EndTransition(const VulkanRHLPendingTransition &pendingTransition) {
    ProcessTransition(pendingTransition, true, false);
    if (pendingTransition.IsCrossQueue())
        AddWaitSemaphore(pendingTransition.GetTransitionSemaphore(), vk::PipelineStageFlagBits2::eAllCommands);
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