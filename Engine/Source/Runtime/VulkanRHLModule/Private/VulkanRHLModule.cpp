#include "../Public/VulkanRHLModule.h"
#include "VulkanRHLFactory.h"

Photon::Result VulkanRHLModule::StartUp() {
    m_vulkanRHLFactory = new VulkanRHLFactory();
    InOutCreateParams<Photon::Result> inOutCreateParams{};
    m_vulkanDynamicRHL = m_vulkanRHLFactory->CreateVulkanDynamicRHL(VulkanDynamicRHLCreateInfo{}, &inOutCreateParams);
    return inOutCreateParams.m_result;
}

void VulkanRHLModule::Terminate() {
    delete m_vulkanRHLFactory;
    delete m_vulkanDynamicRHL;
}

VulkanDynamicRHL * VulkanRHLModule::GetVulkanDynamicRHL() const {
    return m_vulkanDynamicRHL;
}