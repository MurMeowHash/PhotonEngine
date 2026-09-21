#include "../Public/VulkanRHLLoop.h"
#include "VulkanGlobals.h"
#include "VulkanRHLFactory.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

bool VulkanRHLLoop::Initialize() {
    bool isRHLValid;
    g_vulkanDynamicRHL = Photon::Vulkan::RHLFactory::CreateVulkanDynamicRHL(&isRHLValid);
    return isRHLValid;
}

bool VulkanRHLLoop::Tick() {
    return true;
}

bool VulkanRHLLoop::Exit() {
    delete g_vulkanDynamicRHL;
    return true;
}