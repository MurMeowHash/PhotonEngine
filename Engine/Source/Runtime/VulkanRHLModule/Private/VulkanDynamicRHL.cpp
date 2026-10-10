#include "../Public/VulkanDynamicRHL.h"
#include "BuildConfiguration.h"
#include "Configurations/VulkanDeviceConfiguration.h"
#include "Configurations/VulkanExtensionsConfiguration.h"
#include "Configurations/VulkanInstanceConfiguration.h"
#include "Device/VulkanDeviceProvider.h"

VulkanDynamicRHL::~VulkanDynamicRHL() {
    delete m_immediateRHlCommandList;
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

VulkanDevice* VulkanDynamicRHL::GetVulkanDevice() const {
    return m_vulkanDevice;
}

VulkanRHLCommandList* VulkanDynamicRHL::GetImmediateRHLCommandList() const {
    return m_immediateRHlCommandList;
}

void VulkanDynamicRHL::TransitionTexture(VulkanRHLTexture *rhlTexture, RHLPipelineUsage srcUsage, RHLPipelineUsage dstUsage) const {
    uint32_t srcQueueFamilyIndex, dstQueueFamilyIndex;
    ResolveTransitionQueueFamilyIndices(srcUsage, dstUsage, srcQueueFamilyIndex, dstQueueFamilyIndex);
    ImageTransitionScope srcScope = ResolveImageTransitionScopeFromUsage(srcUsage, srcQueueFamilyIndex);
    ImageTransitionScope dstScope = ResolveImageTransitionScopeFromUsage(dstUsage, dstQueueFamilyIndex);
    VulkanPipelineBarrier barrier;
    barrier.TransitionFullImage(rhlTexture, srcScope, dstScope);
    m_immediateRHlCommandList->GetGraphicsContext()->GetCommandBuffer()->ExecutePipelineBarrier(std::move(barrier));
}

Photon::Result VulkanDynamicRHL::Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo &createInfo) {
    Photon::Result createResult = CreateVulkanInstance();
    if (createResult != Photon::Result::Success)
        return Photon::Result::UnknownFailure;

    createResult = CreateVulkanDevice();
    if (createResult != Photon::Result::Success)
        return Photon::Result::UnknownFailure;

    InOutCreateParams<Photon::Result> commandListInOut;
    m_immediateRHlCommandList = VulkanRHLCommandList::Create({}, &commandListInOut);
    return commandListInOut.m_result;
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
    deviceCreateInfo.m_requestedQueues = vk::QueueFlagBits::eGraphics;
    deviceCreateInfo.m_memoryProviderType = VulkanMemoryProviderType::Default;
    InOutCreateParams<Photon::Result> inOutCreateParams{};
    m_vulkanDevice = VulkanDevice::Create(deviceCreateInfo, &inOutCreateParams);
    return inOutCreateParams.m_result;
}

ImageTransitionScope VulkanDynamicRHL::ResolveImageTransitionScopeFromUsage(RHLPipelineUsage usage, uint32_t queueFamilyIndex) const {
    ImageTransitionScope transitionScope{};
    transitionScope.m_queueFamilyIndex = queueFamilyIndex;
    switch (usage) {
        case RHLPipelineUsage::Unknown:
            transitionScope.m_layout = vk::ImageLayout::eUndefined;
            transitionScope.m_stage = vk::PipelineStageFlagBits2::eAllCommands;
            transitionScope.m_access = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            break;
        case RHLPipelineUsage::RenderOutput:
            transitionScope.m_layout = vk::ImageLayout::eColorAttachmentOptimal;
            transitionScope.m_stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            transitionScope.m_access = vk::AccessFlagBits2::eColorAttachmentWrite;
            break;
        default:
            assert(false);
    }

    return transitionScope;
}

void VulkanDynamicRHL::ResolveTransitionQueueFamilyIndices(RHLPipelineUsage srcUsage, RHLPipelineUsage dstUsage,
    uint32_t &srcQueueFamilyIndex, uint32_t &dstQueueFamilyIndex) const {
    srcQueueFamilyIndex = GetQueueFamilyFromPipeline(srcUsage);
    dstQueueFamilyIndex = GetQueueFamilyFromPipeline(dstUsage);

    if (srcQueueFamilyIndex == vk::QueueFamilyIgnored && dstQueueFamilyIndex != vk::QueueFamilyIgnored)
        srcQueueFamilyIndex = dstQueueFamilyIndex;
    else if (srcQueueFamilyIndex != vk::QueueFamilyIgnored && dstQueueFamilyIndex == vk::QueueFamilyIgnored)
        dstQueueFamilyIndex = srcQueueFamilyIndex;
}

uint32_t VulkanDynamicRHL::GetQueueFamilyFromPipeline(RHLPipelineUsage pipelineUsage) const {
    vk::QueueFlagBits queueType;
    switch (pipelineUsage) {
        case RHLPipelineUsage::RenderOutput:
            queueType = vk::QueueFlagBits::eGraphics;
            break;
        default:
            return vk::QueueFamilyIgnored;
    }

    VulkanQueue* queue;
    bool queueAcquired = m_vulkanDevice->TryGetQueue(queueType, queue);
    return queueAcquired ? queue->GetQueueFamilyIndex() : vk::QueueFamilyIgnored;
}