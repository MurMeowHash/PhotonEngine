#include "../Public/VulkanRHLFactory.h"

VulkanDynamicRHL * Photon::Vulkan::RHLFactory::CreateVulkanDynamicRHL(bool *isValid) {
    VulkanDynamicRHL* dynamicRHL = new VulkanDynamicRHL();
    bool isInitialized = dynamicRHL->Initialize();
    if (isValid)
        *isValid = isInitialized;

    return dynamicRHL;
}