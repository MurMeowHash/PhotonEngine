#include "../../Public/Resources/VulkanResource.h"
#include "Synchronization/VulkanPipelineBarrier.h"

vk::PipelineStageFlags2 VulkanResource::GetLastPipelineStage() const {
    return m_lastPipelineStage;
}

vk::AccessFlags2 VulkanResource::GetLastAccess() const {
    return m_lastAccess;
}

void VulkanResource::CommitResourceTransitionScope(const VulkanPipelineResourceTransitionScope &transitionScope) {
    m_lastPipelineStage = transitionScope.m_stage;
    m_lastAccess = transitionScope.m_access;
    m_lastQueueFamily = transitionScope.m_queueFamily;
}
