#pragma once
#include "VulkanInstance.h"

namespace Photon::Vulkan::InstanceFactory {
    VulkanInstance* CreateVulkanInstance(const VulkanInstanceCreateInfo& createInfo, bool* isValid = nullptr);
    VulkanDebugger* CreateVulkanDebugger(const VulkanDebuggerCreateInfo& createInfo, bool* isValid = nullptr);
}
