#include "../../Public/Synchronization/VulkanPipelineBarrier.h"
#include "Resources/VulkanRHLTexture.h"

void VulkanPipelineBarrier::TransitionImage(VulkanRHLTexture *vulkanImage, const VulkanPipelineImageTransitionScope &srcScope,
    const VulkanPipelineImageTransitionScope &destScope, vk::ImageSubresourceRange subresourceRange) {
    vk::ImageMemoryBarrier2 imageMemoryBarrier{};
    imageMemoryBarrier.srcStageMask = srcScope.m_stage;
    imageMemoryBarrier.srcAccessMask = srcScope.m_access;
    imageMemoryBarrier.dstStageMask = destScope.m_stage;
    imageMemoryBarrier.dstAccessMask = destScope.m_access;
    imageMemoryBarrier.oldLayout = srcScope.m_layout;
    imageMemoryBarrier.newLayout = destScope.m_layout;
    imageMemoryBarrier.srcQueueFamilyIndex = srcScope.m_queueFamily;
    imageMemoryBarrier.dstQueueFamilyIndex = destScope.m_queueFamily;
    imageMemoryBarrier.image = vulkanImage->GetHandle();
    imageMemoryBarrier.subresourceRange = subresourceRange;
    m_imageMemoryBarriers.emplace_back(imageMemoryBarrier);
    AddImageTransitionTracking(vulkanImage, destScope);
}

VulkanTransitionInfo VulkanPipelineBarrier::PackTransitionInfo() {
    VulkanTransitionInfo transitionInfo{};
    transitionInfo.m_imageMemoryBarriers = std::move(m_imageMemoryBarriers);
    transitionInfo.m_pendingResources = std::move(m_pendingResources);
    transitionInfo.m_dependencyInfo.imageMemoryBarrierCount = transitionInfo.m_imageMemoryBarriers.size();
    transitionInfo.m_dependencyInfo.pImageMemoryBarriers = transitionInfo.m_imageMemoryBarriers.data();

    Reset();
    return transitionInfo;
}

void VulkanPipelineBarrier::Reset() {
    m_imageMemoryBarriers.clear();
    m_pendingResources.clear();
}

void VulkanPipelineBarrier::AddImageTransitionTracking(VulkanRHLTexture *rhlTexture, const VulkanPipelineImageTransitionScope &destScope) {
    m_pendingResources.emplace_back(rhlTexture);
    rhlTexture->TrackLastTransitionScope(destScope);
}