#pragma once
#include "VulkanDynamicRHL.h"
#include "IVulkanRHLFactory.h"

class VulkanRHLFactory : public IVulkanRHLFactory {
public:
    VulkanDynamicRHL* CreateVulkanDynamicRHL(const VulkanDynamicRHLCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams) override;
};;