#include "../Public/Resources/VulkanResourceTransition.h"

#include "VulkanUtils.h"
#include "Device/VulkanDevice.h"
#include "Resources/VulkanRHLTexture.h"
#include "Synchronization/VulkanBinarySemaphore.h"

VulkanRHLResourceTransitionRequestBatch::VulkanRHLResourceTransitionRequestBatch(RHLWorkType srcWorkType, RHLWorkType destWorkType)
: m_srcWorkType(srcWorkType), m_destWorkType(destWorkType) { }

void VulkanRHLResourceTransitionRequestBatch::AddTextureTransitionRequest(VulkanRHLTextureTransitionRequest &&textureRequest) {
    m_textureTransitionRequests.emplace_back(std::move(textureRequest));
}

const std::vector<VulkanRHLTextureTransitionRequest> & VulkanRHLResourceTransitionRequestBatch::GetTextureTransitionRequests() const {
    return m_textureTransitionRequests;
}

RHLWorkType VulkanRHLResourceTransitionRequestBatch::GetSrcWorkType() const {
    return m_srcWorkType;
}

RHLWorkType VulkanRHLResourceTransitionRequestBatch::GetDestWorkType() const {
    return m_destWorkType;
}

VulkanRHLPendingTransition::VulkanRHLPendingTransition(VulkanDevice *vulkanDevice, RHLWorkType srcWorkType, RHLWorkType destWorkType)
: m_vulkanDevice(vulkanDevice), m_srcWorkType(srcWorkType), m_destWorkType(destWorkType) {
    m_srcVulkanQueue = GetVulkanQueueByRHLWork(srcWorkType);
    m_destVulkanQueue = GetVulkanQueueByRHLWork(destWorkType);
    if (IsCrossQueue()) {
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

RHLWorkType VulkanRHLPendingTransition::GetSrcWorkType() const {
    return m_srcWorkType;
}

RHLWorkType VulkanRHLPendingTransition::GetDestWorkType() const {
    return m_destWorkType;
}

uint32_t VulkanRHLPendingTransition::GetSrcQueueFamily() const {
    return m_srcVulkanQueue == nullptr ? vk::QueueFamilyIgnored : m_srcVulkanQueue->GetQueueFamilyIndex();
}

uint32_t VulkanRHLPendingTransition::GetDestQueueFamily() const {
    return m_destVulkanQueue == nullptr ? vk::QueueFamilyIgnored : m_destVulkanQueue->GetQueueFamilyIndex();
}

bool VulkanRHLPendingTransition::IsCrossQueue() const {
    return m_srcVulkanQueue != m_destVulkanQueue;
}

VulkanBinarySemaphore * VulkanRHLPendingTransition::GetTransitionSemaphore() const {
    return m_transitionSemaphore; // TODO: how to dispose
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

VulkanQueue* VulkanRHLPendingTransition::GetVulkanQueueByRHLWork(RHLWorkType workType) const {
    VulkanQueue* queue;
    bool acquiredQueue = m_vulkanDevice->TryGetQueue(GetQueueTypeByRHLWork(workType), queue);
    return acquiredQueue ? queue : nullptr;
}