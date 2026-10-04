#include "../../Public/Instance/VulkanInstance.h"
#include "CoreUtils.h"
#include "EngineDataConfiguration.h"
#include "VulkanUtils.h"
#include "Configurations/VulkanExtensionsConfiguration.h"

using Photon::Core::Flags::operator|;

VulkanInstance::~VulkanInstance() {
    delete m_vulkanDebugger;
}

bool VulkanInstance::TryGetAvailableExtensions(std::vector<vk::ExtensionProperties> &availableExtensions) const {
    vk::ResultValue<std::vector<vk::ExtensionProperties>> extensionsQueryResult = m_vulkanContext.enumerateInstanceExtensionProperties();
    if (extensionsQueryResult.result != vk::Result::eSuccess)
        return false;

    availableExtensions = extensionsQueryResult.value;
    return true;
}

VulkanInstance* VulkanInstance::Create(const VulkanInstanceCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanInstance* instance = Photon::AllocateObject<VulkanInstance>(inOutCreateParams);
    uint32_t apiVersion = Photon::Vulkan::EngineToVulkanApiVersion(Photon::EngineDataConfiguration::g_vulkanVersion);
    instance->SetApiVersion(apiVersion);
    vk::ApplicationInfo appInfo{};
    appInfo.pEngineName = Photon::EngineDataConfiguration::g_engineName;
    appInfo.engineVersion = Photon::Vulkan::EngineToVulkanVersion(Photon::EngineDataConfiguration::g_engineVersion);
    appInfo.pApplicationName = Photon::EngineDataConfiguration::g_applicationName;
    appInfo.applicationVersion = Photon::Vulkan::EngineToVulkanVersion(Photon::EngineDataConfiguration::g_applicationVersion);
    appInfo.apiVersion = apiVersion;

    vk::InstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.pApplicationInfo = &appInfo;

    uint32_t droppedLayersCount = 0;
    std::vector<const char*> filteredLayers = instance->FilterValidationLayers(createInfo.m_requestedLayers, droppedLayersCount);
    if (droppedLayersCount != 0 && !Photon::Core::Flags::IsFlagSet(createInfo.m_createFlags, VulkanInstanceCreateFlags::AllowDropLayers)) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return instance;
    }

    instanceCreateInfo.ppEnabledLayerNames = filteredLayers.data();
    instanceCreateInfo.enabledLayerCount = filteredLayers.size();

    instance->ProcessExtensionsRequest(createInfo.m_requestedExtensions);
    std::unordered_set<const char*> excludedExtensions;
    if (!instance->IsExtensionSupported(vk::EXTDebugUtilsExtensionName)
        && Photon::Core::Flags::IsFlagSet(createInfo.m_createFlags, VulkanInstanceCreateFlags::AllowDropDebug))
        excludedExtensions.emplace(vk::EXTDebugUtilsExtensionName);

    std::vector<const char*> targetExtensions = instance->QueryExtensionNames(VulkanExtensionQueryFilter::Supported | VulkanExtensionQueryFilter::Required, &excludedExtensions);
    instanceCreateInfo.ppEnabledExtensionNames = targetExtensions.data();
    instanceCreateInfo.enabledExtensionCount = targetExtensions.size();

    vk::ResultValue<vk::raii::Instance> instanceCreateWrapper = instance->m_vulkanContext.createInstance(instanceCreateInfo);
    if (instanceCreateWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return instance;
    }

    instance->m_handle = std::move(instanceCreateWrapper.value);
    VulkanDebuggerCreateInfo debuggerCreateInfo {instance};
    InOutCreateParams<Photon::Result> debuggerInOutCreateParams{};
    VulkanDebugger* debugger = VulkanDebugger::Create(debuggerCreateInfo, &debuggerInOutCreateParams);
    if (debuggerInOutCreateParams.m_result != Photon::Result::Success
        && !Photon::Core::Flags::IsFlagSet(createInfo.m_createFlags, VulkanInstanceCreateFlags::AllowDropDebug)) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return instance;
    }

    instance->m_vulkanDebugger = debugger;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

const vk::raii::Instance & VulkanInstance::GetHandle() const {
    return m_handle;
}

std::vector<const char *> VulkanInstance::FilterValidationLayers(const std::vector<const char *> &requestedLayers, uint32_t &droppedLayersCount) const {
    vk::ResultValue<std::vector<vk::LayerProperties>> installedLayers = m_vulkanContext.enumerateInstanceLayerProperties();
    if (installedLayers.result != vk::Result::eSuccess) {
        droppedLayersCount = requestedLayers.size();
        return {};
    }

    std::vector<const char*> intersectedLayers;
    intersectedLayers.reserve(installedLayers.value.size());

    for(const char* layer : requestedLayers) {
        auto layerIterator = std::find_if(installedLayers.value.begin(), installedLayers.value.end(),
                                              [layer](const vk::LayerProperties &installedLayer) {
                                                  return std::strcmp(layer, installedLayer.layerName) == 0;
                                              });

        if (layerIterator == installedLayers.value.end())
            droppedLayersCount++;
        else
            intersectedLayers.emplace_back(layer);
    }

    return intersectedLayers;
}