#include "../Public/VulkanRHLGraphicsWorkContext.h"
#include "VulkanDynamicRHL.h"
#include "VulkanGlobals.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

VulkanRHLGraphicsWorkContext* VulkanRHLGraphicsWorkContext::Create([[maybe_unused]] const VulkanRHLGraphicsWorkContextCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLGraphicsWorkContext* instance = Photon::AllocateObject<VulkanRHLGraphicsWorkContext>(inOutCreateParams);
    Photon::Result initializeResult = instance->InitializeContext(RHLPipeline::Graphics);
    Photon::PushResult(initializeResult, inOutCreateParams);
    return instance;
}