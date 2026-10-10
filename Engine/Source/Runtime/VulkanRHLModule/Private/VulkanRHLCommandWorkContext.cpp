#include "../Public/VulkanRHLCommandWorkContext.h"
#include "VulkanDynamicRHL.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

Photon::Result VulkanRHLCommandWorkContext::InitializeContext() {
    if (g_vulkanDynamicRHL == nullptr)
        return Photon::Result::UnknownFailure;

    VulkanQueue* queue;
    if (!g_vulkanDynamicRHL->GetVulkanDevice()->TryGetQueue(GetQueueTypeByRHLWork(GetVulkanRHLWorkType()), queue))
        return Photon::Result::UnknownFailure;

    Initialize(queue, g_vulkanDynamicRHL->GetVulkanDevice());
    return Photon::Result::Success;
}

Photon::Result VulkanRHLCommandWorkContext::FinalizeWork() {
    VulkanWorkSubmitInfo submitInfo;
    Photon::Result packResult = PackWorkBatches(submitInfo);
    if (packResult != Photon::Result::Success)
        return packResult;

    return m_workQueue->SubmitWorkBatches(std::move(submitInfo));
}

vk::QueueFlagBits VulkanRHLCommandWorkContext::GetQueueTypeByRHLWork(Photon::Vulkan::RHLWorkType rhlWorkType) {
    switch (rhlWorkType) {
        case Photon::Vulkan::RHLWorkType::Graphics:
            return vk::QueueFlagBits::eGraphics;
        default:
            throw std::runtime_error(StringFormatter("Failed to get vulkan queue by RHL, ", static_cast<uint32_t>(rhlWorkType)));
    }
}
