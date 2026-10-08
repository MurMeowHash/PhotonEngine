#include "../../Public/Queue/VulkanWorkContextPool.h"

VulkanWorkContext* VulkanWorkContextPool::CreateObject(InOutCreateParams<Photon::Result>* inOutCreateParams) {
    VulkanWorkContextCreateInfo createInfo{};
    createInfo.m_vulkanQueue = m_vulkanQueue;
    return VulkanWorkContext::Create(createInfo, inOutCreateParams);
}

void VulkanWorkContextPool::DisposeObject(VulkanWorkContext *object) {
    PhotonPool<VulkanWorkContext>::DisposeObject(object);
    object->m_workBatches.clear();
    object->m_isBatchesPacked = false;
    object->m_currentStage = VulkanWorkStage::Wait;
    object->m_workAllocator->InvalidateAllocator();
}

VulkanWorkContextPool* VulkanWorkContextPool::Create(const VulkanWorkContextPoolCreateInfo &createInfo,
                                                     InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanWorkContextPool* instance = Photon::AllocateObject<VulkanWorkContextPool>(inOutCreateParams);
    instance->Initialize(0, UINT32_MAX);
    instance->m_vulkanQueue = createInfo.m_vulkanQueue;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}
