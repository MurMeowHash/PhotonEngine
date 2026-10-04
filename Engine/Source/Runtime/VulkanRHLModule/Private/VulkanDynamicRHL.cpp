#include "../Public/VulkanDynamicRHL.h"
#include "BuildConfiguration.h"
#include "Configurations/VulkanDeviceConfiguration.h"
#include "Configurations/VulkanExtensionsConfiguration.h"
#include "Configurations/VulkanInstanceConfiguration.h"
#include "Device/VulkanDeviceProvider.h"

using Photon::Core::Flags::operator|=;
using Photon::Core::Flags::operator|;

VulkanDynamicRHL::~VulkanDynamicRHL() {
    delete m_vulkanDevice;
    delete m_vulkanInstance;
}

VulkanRHLViewport* VulkanDynamicRHL::CreateVulkanRHLViewport(const RHLViewportCreateInfo &createInfo,
    InOutCreateParams<Photon::Result>* inOutCreateParams) const {
    VulkanRHLViewportCreateInfo vulkanCreateInfo{};
    vulkanCreateInfo.m_vulkanInstance = m_vulkanInstance;
    vulkanCreateInfo.m_vulkanDevice = m_vulkanDevice;
    vulkanCreateInfo.m_viewportInfo = createInfo;
    return VulkanRHLViewport::Create(vulkanCreateInfo, inOutCreateParams);
}

Photon::Result VulkanDynamicRHL::Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo &createInfo) {
    Photon::Result createResult = CreateVulkanInstance();
    if (createResult != Photon::Result::Success)
        return Photon::Result::UnknownFailure;

    createResult = CreateVulkanDevice();
    return createResult;
}

Photon::Result VulkanDynamicRHL::CreateVulkanInstance() {
    VulkanInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.m_requestedExtensions = Photon::Vulkan::ExtensionsConfiguration::g_instanceExtensions;
    instanceCreateInfo.m_requestedLayers = Photon::Vulkan::VulkanInstanceConfiguration::g_validationLayers;
    instanceCreateInfo.m_createFlags = VulkanInstanceCreateFlags::None;
    if (Photon::BuildConfiguration::g_buildType == BuildType::Development)
        instanceCreateInfo.m_createFlags |= VulkanInstanceCreateFlags::UseDebug;

    InOutCreateParams<Photon::Result> inOutCreateParams{};
    m_vulkanInstance = VulkanInstance::Create(instanceCreateInfo, &inOutCreateParams);
    return inOutCreateParams.m_result;
}

Photon::Result VulkanDynamicRHL::CreateVulkanDevice() {
    vk::raii::PhysicalDevice suitableDevice = nullptr;
    if (!Photon::Vulkan::DeviceProvider::TryFindVulkanDevice(m_vulkanInstance->GetHandle(), 0,
        Photon::Vulkan::DeviceSearchFlags::AllowNonGpu | Photon::Vulkan::DeviceSearchFlags::RenderingOnly, &suitableDevice))
        return Photon::Result::UnknownFailure;

    VulkanDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.m_physicalDevice = suitableDevice;
    deviceCreateInfo.m_requestedExtensions = Photon::Vulkan::ExtensionsConfiguration::g_deviceExtensions;
    deviceCreateInfo.m_requestedFeatures = Photon::Vulkan::DeviceConfiguration::g_deviceFeatures;
    deviceCreateInfo.m_requestedQueues = vk::QueueFlagBits::eGraphics;
    deviceCreateInfo.m_memoryProviderType = VulkanMemoryProviderType::Default;
    InOutCreateParams<Photon::Result> inOutCreateParams{};
    m_vulkanDevice = VulkanDevice::Create(deviceCreateInfo, &inOutCreateParams);
    return inOutCreateParams.m_result;
}