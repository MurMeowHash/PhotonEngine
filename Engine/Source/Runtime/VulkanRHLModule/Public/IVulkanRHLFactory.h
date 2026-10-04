#pragma once
#include "VulkanDynamicRHL.h"

class IVulkanRHLFactory {
public:
    virtual ~IVulkanRHLFactory() = default;
    virtual VulkanDynamicRHL* CreateVulkanDynamicRHL(const VulkanDynamicRHLCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) = 0;
};
