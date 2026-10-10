#pragma once

#include "VulkanWorkContext.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Pools/PhotonPool.h"

class VulkanQueue;
class IAllocator;

struct VulkanWorkAllocatorPoolCreateInfo {

};

class VulkanWorkAllocatorPool : public PhotonPool<IAllocator> {
protected:
    IAllocator* CreateObject(InOutCreateParams<Photon::Result> *inOutCreateParams) override;
    void DisposeObject(IAllocator *object) override;

public:
    static VulkanWorkAllocatorPool* Create([[maybe_unused]] const VulkanWorkAllocatorPoolCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
};