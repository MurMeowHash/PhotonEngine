#include "../Public/VulkanRHLFactory.h"

VulkanDynamicRHL* VulkanRHLFactory::CreateVulkanDynamicRHL(const VulkanDynamicRHLCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams) {
    VulkanDynamicRHL* vulkanDynamicRHL = Photon::AllocateObject<VulkanDynamicRHL>(inOutCreateParams);
    Photon::Result rhlCreateResult = vulkanDynamicRHL->Create(createInfo);
    Photon::PushResult(rhlCreateResult, inOutCreateParams);
    return vulkanDynamicRHL;
}