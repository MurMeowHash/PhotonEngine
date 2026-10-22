#include "../../Public/Systems/VulkanRHLDDQProcessSystem.h"
#include "VulkanDynamicRHL.h"

void VulkanRHLDDQProcessSystem::Initialize(VulkanDynamicRHL *dynamicRHL) {
    VulkanRHLSystemBase::Initialize(dynamicRHL);
    m_ddq = m_dynamicRHL->GetVulkanDevice()->GetDeferredDeletionQueue();
}

void VulkanRHLDDQProcessSystem::Tick() {
    m_ddq->StampPendingEntries();
    m_ddq->ProcessRelease();
}

void VulkanRHLDDQProcessSystem::Dispose() {
    m_ddq->ReleaseAll();
}