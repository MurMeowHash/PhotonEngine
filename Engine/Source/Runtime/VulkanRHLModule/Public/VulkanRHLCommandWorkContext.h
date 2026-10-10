#pragma once

#include "CoreGlobals.h"
#include "VulkanGlobals.h"
#include "Queue/VulkanWorkContext.h"

class VulkanRHLCommandWorkContext : public VulkanWorkContext {
public:
    [[nodiscard]] Photon::Result FinalizeWork();
protected:
    virtual Photon::Vulkan::RHLWorkType GetVulkanRHLWorkType() = 0;
    [[nodiscard]] Photon::Result InitializeContext();
private:
    static vk::QueueFlagBits GetQueueTypeByRHLWork(Photon::Vulkan::RHLWorkType rhlWorkType);
};
