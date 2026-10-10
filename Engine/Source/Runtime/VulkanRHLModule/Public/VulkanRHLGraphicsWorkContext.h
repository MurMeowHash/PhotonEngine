#pragma once
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanRHLCommandWorkContext.h"

struct VulkanRHLGraphicsWorkContextCreateInfo {

};

class VulkanRHLGraphicsWorkContext : public VulkanRHLCommandWorkContext {
protected:
    Photon::Vulkan::RHLWorkType GetVulkanRHLWorkType() override;

public:
    static VulkanRHLGraphicsWorkContext* Create([[maybe_unused]] const VulkanRHLGraphicsWorkContextCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
};