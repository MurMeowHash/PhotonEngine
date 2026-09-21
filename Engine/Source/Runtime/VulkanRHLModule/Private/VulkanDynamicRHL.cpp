#include "../Public/VulkanDynamicRHL.h"

#include "BuildConfiguration.h"
#include "Configurations/VulkanDeviceConfiguration.h"
#include "Configurations/VulkanExtensionsConfiguration.h"
#include "Configurations/VulkanInstanceConfiguration.h"
#include "Device/VulkanDeviceFactory.h"
#include "Device/VulkanDeviceProvider.h"
#include "Instance/VulkanInstanceFactory.h"

using Photon::Core::Flags::operator|=;
using Photon::Core::Flags::operator|;

VulkanDynamicRHL::~VulkanDynamicRHL() {
    delete m_vulkanDevice;
    delete m_vulkanInstance;
}

bool VulkanDynamicRHL::Initialize() {
    if (!CreateVulkanInstance())
        return false;

    if (!CreateVulkanDevice())
        return false;

    return true;
}

bool VulkanDynamicRHL::CreateVulkanInstance() {
    VulkanInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.m_requestedExtensions = Photon::Vulkan::ExtensionsConfiguration::g_instanceExtensions;
    instanceCreateInfo.m_requestedLayers = Photon::Vulkan::VulkanInstanceConfiguration::g_validationLayers;
    instanceCreateInfo.m_createFlags = VulkanInstanceCreateFlags::None;
    if (Photon::BuildConfiguration::g_buildType == BuildType::Development)
        instanceCreateInfo.m_createFlags |= VulkanInstanceCreateFlags::UseDebug;

    bool isInstanceValid;
    m_vulkanInstance = Photon::Vulkan::InstanceFactory::CreateVulkanInstance(instanceCreateInfo, &isInstanceValid);
    return isInstanceValid;
}

bool VulkanDynamicRHL::CreateVulkanDevice() {
    vk::raii::PhysicalDevice suitableDevice = nullptr;
    if (!Photon::Vulkan::DeviceProvider::TryFindVulkanDevice(m_vulkanInstance->GetHandle(), 0,
        Photon::Vulkan::DeviceSearchFlags::AllowNonGpu | Photon::Vulkan::DeviceSearchFlags::RenderingOnly, &suitableDevice))
        return false;

    VulkanDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.m_physicalDevice = suitableDevice;
    deviceCreateInfo.m_requestedExtensions = Photon::Vulkan::ExtensionsConfiguration::g_deviceExtensions;
    deviceCreateInfo.m_requestedFeatures = Photon::Vulkan::DeviceConfiguration::g_deviceFeatures;
    deviceCreateInfo.m_requestedQueues = vk::QueueFlagBits::eGraphics;
    deviceCreateInfo.m_memoryProviderType = VulkanMemoryProviderType::Default;
    bool isDeviceValid;
    m_vulkanDevice = Photon::Vulkan::DeviceFactory::CreateVulkanDevice(deviceCreateInfo, &isDeviceValid);
    return isDeviceValid;
}
