#include "../../Public/Resources/VulkanRHLResource.h"
#include "Synchronization/VulkanPipelineBarrier.h"

vk::PipelineStageFlags2 VulkanRHLResource::GetLastPipelineStage() const {
    return m_lastPipelineStage;
}

vk::AccessFlags2 VulkanRHLResource::GetLastAccess() const {
    return m_lastAccess;
}

void VulkanRHLResource::CommitResourceTransitionScope(const VulkanPipelineResourceTransitionScope &transitionScope) {
    m_lastPipelineStage = transitionScope.m_stage;
    m_lastAccess = transitionScope.m_access;
    m_lastQueueFamily = transitionScope.m_queueFamily;
}
