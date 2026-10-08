#pragma once

#include "VulkanWorkContext.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Pools/PhotonPool.h"

class VulkanQueue;

struct VulkanWorkContextPoolCreateInfo {
    VulkanQueue* m_vulkanQueue;
};

class VulkanWorkContextPool : public PhotonPool<VulkanWorkContext> {
protected:
    VulkanWorkContext* CreateObject(InOutCreateParams<Photon::Result>* inOutCreateParams) override;
    void DisposeObject(VulkanWorkContext *object) override;

public:
    static VulkanWorkContextPool* Create(const VulkanWorkContextPoolCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

private:
    VulkanQueue* m_vulkanQueue = nullptr;
};
