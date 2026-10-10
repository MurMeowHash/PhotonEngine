#include "../../Public/Queue/VulkanWorkPools.h"
#include "LinearAllocator.h"

void VulkanWorkAllocatorPool::DisposeObject(IAllocator *object) {
    PhotonPool<IAllocator>::DisposeObject(object);
    object->InvalidateAllocator();
}

VulkanWorkAllocatorPool* VulkanWorkAllocatorPool::Create([[maybe_unused]] const VulkanWorkAllocatorPoolCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanWorkAllocatorPool* instance = Photon::AllocateObject<VulkanWorkAllocatorPool>(inOutCreateParams);
    instance->Initialize(0, UINT32_MAX);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

IAllocator* VulkanWorkAllocatorPool::CreateObject(InOutCreateParams<Photon::Result> *inOutCreateParams) {
    return Photon::AllocateObject<LinearAllocator>(inOutCreateParams, sizeof(VulkanWorkBatch) * 8ull);
}