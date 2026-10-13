#pragma once
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanRHLComputeWorkContext.h"

struct VulkanRHLGraphicsWorkContextCreateInfo {

};

class VulkanRHLGraphicsWorkContext : public VulkanRHLComputeWorkContext {
public:
    static VulkanRHLGraphicsWorkContext* Create([[maybe_unused]] const VulkanRHLGraphicsWorkContextCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
};