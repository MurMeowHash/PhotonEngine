#include "../Public/Resources/VulkanResourceTransition.h"

#include "Device/VulkanDevice.h"
#include "Resources/VulkanRHLTexture.h"
#include "Synchronization/VulkanBinarySemaphore.h"

VulkanRHLResourceTransitionRequestBatch::VulkanRHLResourceTransitionRequestBatch(RHLPipeline srcPipeline, RHLPipeline destPipeline)
: m_srcPipeline(srcPipeline), m_destPipeline(destPipeline) { }

void VulkanRHLResourceTransitionRequestBatch::AddTextureTransitionRequest(VulkanRHLTextureTransitionRequest &&textureRequest) {
    m_textureTransitionRequests.emplace_back(std::move(textureRequest));
}

const std::vector<VulkanRHLTextureTransitionRequest> & VulkanRHLResourceTransitionRequestBatch::GetTextureTransitionRequests() const {
    return m_textureTransitionRequests;
}

RHLPipeline VulkanRHLResourceTransitionRequestBatch::GetSrcPipeline() const {
    return m_srcPipeline;
}

RHLPipeline VulkanRHLResourceTransitionRequestBatch::GetDestPipeline() const {
    return m_destPipeline;
}

VulkanRHLPendingTransition::VulkanRHLPendingTransition(VulkanDevice *vulkanDevice, RHLPipeline srcPipeline, RHLPipeline destPipeline)
: m_vulkanDevice(vulkanDevice), m_srcPipeline(srcPipeline), m_destPipeline(destPipeline) {
    m_srcVulkanQueue = GetVulkanQueueByRHLPipeline(srcPipeline);
    m_destVulkanQueue = GetVulkanQueueByRHLPipeline(destPipeline);
    if (IsCrossPipeline()) {
        m_vulkanDevice->TryAllocateSemaphore(m_transitionSemaphore);
    }
}

void VulkanRHLPendingTransition::CreateTexturePendingTransition(const VulkanRHLTextureTransitionRequest &textureRequest) {
    VulkanImagePendingTransition pendingTransition{};
    pendingTransition.m_rhlTexture = textureRequest.m_rhlTexture;
    pendingTransition.m_srcScope = CreateImageTransitionScope(textureRequest.m_srcUsage);
    pendingTransition.m_destScope = CreateImageTransitionScope(textureRequest.m_destUsage);
    pendingTransition.m_subresourceRange = textureRequest.m_rhlTexture->GetDefaultImageView()->GetSubresourceRange();
    m_imagePendingTransitions.emplace_back(pendingTransition);
}

const std::vector<VulkanImagePendingTransition> & VulkanRHLPendingTransition::GetImagePendingTransitions() const {
    return m_imagePendingTransitions;
}

RHLPipeline VulkanRHLPendingTransition::GetSrcPipeline() const {
    return m_srcPipeline;
}

RHLPipeline VulkanRHLPendingTransition::GetDestPipeline() const {
    return m_destPipeline;
}

uint32_t VulkanRHLPendingTransition::GetSrcQueueFamily() const {
    return m_srcVulkanQueue == nullptr ? vk::QueueFamilyIgnored : m_srcVulkanQueue->GetQueueFamilyIndex();
}

uint32_t VulkanRHLPendingTransition::GetDestQueueFamily() const {
    return m_destVulkanQueue == nullptr ? vk::QueueFamilyIgnored : m_destVulkanQueue->GetQueueFamilyIndex();
}

bool VulkanRHLPendingTransition::IsCrossPipeline() const {
    return m_srcPipeline != m_destPipeline;
}

std::shared_ptr<VulkanBinarySemaphore> VulkanRHLPendingTransition::GetTransitionSemaphore() const {
    return m_transitionSemaphore;
}

void VulkanRHLPendingTransition::FillBasicResourceTransitionScope(VulkanResourceTransitionScope &transitionScope, RHLPipelineUsage usage) {
    switch (usage) {
        case RHLPipelineUsage::Unknown:
            transitionScope.m_stage = vk::PipelineStageFlagBits2::eAllCommands;
            transitionScope.m_access = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            break;
        case RHLPipelineUsage::RenderTarget:
            transitionScope.m_stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            transitionScope.m_access = vk::AccessFlagBits2::eColorAttachmentWrite;
            break;
        default:
            assert(false);
    }
}

void VulkanRHLPendingTransition::GetTransitionImageLayout(RHLPipelineUsage usage, vk::ImageLayout& layout) {
    switch (usage) {
        case RHLPipelineUsage::Unknown:
            layout = vk::ImageLayout::eUndefined;
        case RHLPipelineUsage::RenderTarget:
            layout = vk::ImageLayout::eColorAttachmentOptimal;
        default:
            assert(false);
    }
}

VulkanImageTransitionScope VulkanRHLPendingTransition::CreateImageTransitionScope(RHLPipelineUsage usage) {
    VulkanImageTransitionScope scope{};
    FillBasicResourceTransitionScope(scope, usage);
    GetTransitionImageLayout(usage, scope.m_layout);
    return scope;
}

VulkanQueue* VulkanRHLPendingTransition::GetVulkanQueueByRHLPipeline(RHLPipeline pipeline) const {
    VulkanQueue* queue;
    bool acquiredQueue = m_vulkanDevice->TryGetQueue(pipeline, queue);
    return acquiredQueue ? queue : nullptr;
}