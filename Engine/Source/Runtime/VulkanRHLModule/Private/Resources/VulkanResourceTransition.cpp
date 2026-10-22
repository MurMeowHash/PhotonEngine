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
    pendingTransition.m_srcScope = CreateSrcImageTransitionScope(textureRequest.m_rhlTexture, textureRequest.m_srcUsage);
    pendingTransition.m_destScope = CreateDestImageTransitionScope(textureRequest.m_destUsage);
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

void VulkanRHLPendingTransition::FillBasicResourceTransitionScope(RHLPipelineUsage usage, VulkanResourceTransitionScope &transitionScope) {
    switch (usage) {
        case RHLPipelineUsage::DontCare:
        case RHLPipelineUsage::Unknown:
            transitionScope.m_stage = vk::PipelineStageFlagBits2::eAllCommands;
            transitionScope.m_access = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
            break;
        case RHLPipelineUsage::RenderTarget:
            transitionScope.m_stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            transitionScope.m_access = vk::AccessFlagBits2::eColorAttachmentWrite;
            break;
        case RHLPipelineUsage::Present:
            transitionScope.m_stage = vk::PipelineStageFlagBits2::eNone;
            transitionScope.m_access = vk::AccessFlagBits2::eNone;
            break;
        default:
            assert(false);
    }
}

void VulkanRHLPendingTransition::FillBasicSrcResourceTransitionScope(VulkanResource *rhlResource,
    RHLPipelineUsage usage, VulkanResourceTransitionScope &transitionScope) {
    switch (usage) {
        case RHLPipelineUsage::Unknown:
            transitionScope.m_stage = rhlResource->GetLastPipelineStage();
            transitionScope.m_access = rhlResource->GetLastAccess();
            break;
        default:
            FillBasicResourceTransitionScope(usage, transitionScope);
    }
}

void VulkanRHLPendingTransition::FillBasicDestResourceTransitionScope(RHLPipelineUsage usage, VulkanResourceTransitionScope &transitionScope) {
    return FillBasicResourceTransitionScope(usage, transitionScope);
}

void VulkanRHLPendingTransition::GetTransitionImageLayout(RHLPipelineUsage usage, vk::ImageLayout& layout) {
    switch (usage) {
        case RHLPipelineUsage::DontCare:
        case RHLPipelineUsage::Unknown:
            layout = vk::ImageLayout::eUndefined;
            break;
        case RHLPipelineUsage::RenderTarget:
            layout = vk::ImageLayout::eColorAttachmentOptimal;
            break;
        case RHLPipelineUsage::Present:
            layout = vk::ImageLayout::ePresentSrcKHR;
            break;
        default:
            assert(false);
    }
}

void VulkanRHLPendingTransition::GetSrcTransitionImageLayout(VulkanRHLTexture* rhlTexture, RHLPipelineUsage usage,
    vk::ImageLayout &layout) {
    switch (usage) {
        case RHLPipelineUsage::Unknown:
            layout = rhlTexture->GetLastLayout();
            break;
        default:
            GetTransitionImageLayout(usage, layout);
    }
}

void VulkanRHLPendingTransition::GetDestTransitionImageLayout(RHLPipelineUsage usage, vk::ImageLayout &layout) {
    return GetTransitionImageLayout(usage, layout);
}

VulkanImageTransitionScope VulkanRHLPendingTransition::CreateSrcImageTransitionScope(VulkanRHLTexture *rhlTexture, RHLPipelineUsage usage) {
    VulkanImageTransitionScope scope{};
    FillBasicSrcResourceTransitionScope(rhlTexture, usage, scope);
    GetSrcTransitionImageLayout(rhlTexture, usage, scope.m_layout);
    return scope;
}

VulkanImageTransitionScope VulkanRHLPendingTransition::CreateDestImageTransitionScope(RHLPipelineUsage usage) {
    VulkanImageTransitionScope scope{};
    FillBasicDestResourceTransitionScope(usage, scope);
    GetDestTransitionImageLayout(usage, scope.m_layout);
    return scope;
}

VulkanQueue* VulkanRHLPendingTransition::GetVulkanQueueByRHLPipeline(RHLPipeline pipeline) const {
    VulkanQueue* queue;
    bool acquiredQueue = m_vulkanDevice->TryGetQueue(pipeline, queue);
    return acquiredQueue ? queue : nullptr;
}