#include "../Public/VulkanDynamicRHL.h"
#include "BuildConfiguration.h"
#include "Configurations/VulkanDeviceConfiguration.h"
#include "Configurations/VulkanExtensionsConfiguration.h"
#include "Configurations/VulkanInstanceConfiguration.h"
#include "Device/VulkanDeviceProvider.h"
#include "Systems/VulkanRHLDDQProcessSystem.h"
#include "Systems/VulkanRHLInterruptProcessSystem.h"

VulkanDynamicRHL::~VulkanDynamicRHL() {
    for (const std::unique_ptr<IVulkanRHLSystem>& rhlSystem : m_rhlSystems)
        rhlSystem->Dispose();

    delete m_vertexLayoutCache;
    delete m_immediateRHlCommandBuffer;
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

void VulkanDynamicRHL::Tick() const {
    for (const std::unique_ptr<IVulkanRHLSystem>& rhlSystem : m_rhlSystems)
        rhlSystem->Tick();
}

VulkanDevice* VulkanDynamicRHL::GetVulkanDevice() const {
    return m_vulkanDevice;
}

VulkanInstance * VulkanDynamicRHL::GetVulkanInstance() const {
    return m_vulkanInstance;
}

VulkanRHLPendingTransition VulkanDynamicRHL::CreateTransition(const VulkanRHLResourceTransitionRequestBatch &transitionRequests) const {
    VulkanRHLPendingTransition pendingTransition{m_vulkanDevice, transitionRequests.GetSrcPipeline(), transitionRequests.GetDestPipeline()};

    for (const VulkanRHLTextureTransitionRequest& textureRequest: transitionRequests.GetTextureTransitionRequests()) {
        pendingTransition.CreateTexturePendingTransition(textureRequest);
    }

    return pendingTransition;
}

VulkanRHLVertexInput VulkanDynamicRHL::CreateVertexInput(const VulkanRHLVertexInputCreateInfo &createInfo) const {
    VulkanRHLVertexLayout* vertexLayout = m_vertexLayoutCache->PoolBlueprint(createInfo.m_vertexLayoutDescriptor);
    VulkanRHLVertexInputInitializeInfo initializeInfo{vertexLayout, createInfo.m_vertexStreams};
    return VulkanRHLVertexInput(initializeInfo);
}

Photon::Result VulkanDynamicRHL::Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo &createInfo) {
    Photon::Result createResult = CreateVulkanInstance();
    if (createResult != Photon::Result::Success)
        return Photon::Result::UnknownFailure;

    createResult = CreateVulkanDevice();
    if (createResult != Photon::Result::Success)
        return Photon::Result::UnknownFailure;

    SetUpRHLSystems();
    for (const std::unique_ptr<IVulkanRHLSystem>& rhlSystem : m_rhlSystems)
        rhlSystem->Initialize(this);

    m_vertexLayoutCache = new VulkanRHLVertexLayoutCache();
    return Photon::Result::Success;
}

Photon::Result VulkanDynamicRHL::CreateVulkanInstance() {
    using Photon::Core::Flags::operator|=;

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
    using Photon::Core::Flags::operator|;

    vk::raii::PhysicalDevice suitableDevice = nullptr;
    if (!Photon::Vulkan::DeviceProvider::TryFindVulkanDevice(m_vulkanInstance->GetHandle(), 0,
        Photon::Vulkan::DeviceSearchFlags::AllowNonGpu | Photon::Vulkan::DeviceSearchFlags::RenderingOnly, &suitableDevice))
        return Photon::Result::UnknownFailure;

    VulkanDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.m_physicalDevice = suitableDevice;
    deviceCreateInfo.m_requestedExtensions = Photon::Vulkan::ExtensionsConfiguration::g_deviceExtensions;
    deviceCreateInfo.m_requestedFeatures = Photon::Vulkan::DeviceConfiguration::g_deviceFeatures;
    deviceCreateInfo.m_requestedPipelines = RHLPipeline::Graphics;
    deviceCreateInfo.m_memoryProviderType = VulkanMemoryProviderType::Default;
    InOutCreateParams<Photon::Result> inOutCreateParams{};
    m_vulkanDevice = VulkanDevice::Create(deviceCreateInfo, &inOutCreateParams);
    return inOutCreateParams.m_result;
}

void VulkanDynamicRHL::SetUpRHLSystems() {
    AddRHLSystem<VulkanRHLInterruptProcessSystem>();
    AddRHLSystem<VulkanRHLDDQProcessSystem>();
}