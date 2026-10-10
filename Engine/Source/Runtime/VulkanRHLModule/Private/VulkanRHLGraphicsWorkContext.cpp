#include "../Public/VulkanRHLGraphicsWorkContext.h"
#include "VulkanDynamicRHL.h"
#include "VulkanGlobals.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

Photon::Vulkan::RHLWorkType VulkanRHLGraphicsWorkContext::GetVulkanRHLWorkType() {
    return Photon::Vulkan::RHLWorkType::Graphics;
}

VulkanRHLGraphicsWorkContext* VulkanRHLGraphicsWorkContext::Create([[maybe_unused]] const VulkanRHLGraphicsWorkContextCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLGraphicsWorkContext* instance = Photon::AllocateObject<VulkanRHLGraphicsWorkContext>(inOutCreateParams);
    Photon::Result initializeResult = instance->InitializeContext();
    Photon::PushResult(initializeResult, inOutCreateParams);
    return instance;
}