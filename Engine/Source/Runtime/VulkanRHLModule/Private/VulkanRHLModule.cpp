#include "../Public/VulkanRHLModule.h"
#include "VulkanRHLFactory.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

Photon::Result VulkanRHLModule::StartUp() {
    m_vulkanRHLFactory = new VulkanRHLFactory();
    InOutCreateParams<Photon::Result> rhlInOut{};
    m_vulkanDynamicRHL = m_vulkanRHLFactory->CreateVulkanDynamicRHL(VulkanDynamicRHLCreateInfo{}, &rhlInOut);
    if (rhlInOut.m_result != Photon::Result::Success)
        return rhlInOut.m_result;

    g_vulkanDynamicRHL = m_vulkanDynamicRHL;
    return rhlInOut.m_result;
}

void VulkanRHLModule::Terminate() {
    delete m_vulkanRHLFactory;
    delete m_vulkanDynamicRHL;
}

VulkanDynamicRHL * VulkanRHLModule::GetVulkanDynamicRHL() const {
    return m_vulkanDynamicRHL;
}