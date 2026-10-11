#pragma once
#include "VulkanBinarySemaphore.h"
#include "Pools/PhotonPool.h"

class VulkanDevice;

struct VulkanBinarySemaphorePoolCreateInfo {
    VulkanDevice* m_vulkanDevice;
};

class VulkanBinarySemaphorePool : public PhotonPool<VulkanBinarySemaphore> {
protected:
    VulkanBinarySemaphore* CreateObject(InOutCreateParams<Photon::Result> *inOutCreateParams) override;

public:
    static VulkanBinarySemaphorePool* Create(const VulkanBinarySemaphorePoolCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

private:
    VulkanDevice* m_vulkanDevice = nullptr;
};
