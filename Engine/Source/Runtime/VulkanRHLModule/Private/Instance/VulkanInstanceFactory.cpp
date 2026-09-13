#include "../../Public/Instance/VulkanInstanceFactory.h"

VulkanInstance* Photon::Vulkan::InstanceFactory::CreateVulkanInstance(const VulkanInstanceCreateInfo &createInfo, bool *isValid) {
    VulkanInstance* instance = new VulkanInstance();
    bool isCreated = instance->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return instance;
}

VulkanDebugger* Photon::Vulkan::InstanceFactory::CreateVulkanDebugger(const VulkanDebuggerCreateInfo &createInfo, bool *isValid) {
    VulkanDebugger* debugger = new VulkanDebugger();
    bool isCreated = debugger->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return debugger;
}
