#pragma once

#include "VulkanWorkContext.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Pools/PhotonPool.h"

class VulkanQueue;
class LinearAllocator;

struct VulkanWorkAllocatorPoolCreateInfo {

};

class VulkanWorkAllocatorPool : public PhotonPool<LinearAllocator> {
protected:
    LinearAllocator* CreateObject(InOutCreateParams<Photon::Result> *inOutCreateParams) override;
    void DisposeObject(LinearAllocator *object) override;

public:
    static VulkanWorkAllocatorPool* Create([[maybe_unused]] const VulkanWorkAllocatorPoolCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
};