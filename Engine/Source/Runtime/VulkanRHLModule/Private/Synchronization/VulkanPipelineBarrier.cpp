#include "../../Public/Synchronization/VulkanPipelineBarrier.h"
#include "Resources/VulkanRHLTexture.h"

void VulkanPipelineBarrier::TransitionImage(VulkanRHLTexture *vulkanImage, ImageTransitionScope srcScope,
                                            ImageTransitionScope dstScope, vk::ImageSubresourceRange subresourceRange) {
    vk::ImageMemoryBarrier2 imageMemoryBarrier{};
    imageMemoryBarrier.srcStageMask = srcScope.m_stage;
    imageMemoryBarrier.srcAccessMask = srcScope.m_access;
    imageMemoryBarrier.dstStageMask = dstScope.m_stage;
    imageMemoryBarrier.dstAccessMask = dstScope.m_access;
    imageMemoryBarrier.oldLayout = srcScope.m_layout;
    imageMemoryBarrier.newLayout = dstScope.m_layout;
    imageMemoryBarrier.srcQueueFamilyIndex = srcScope.m_queueFamilyIndex;
    imageMemoryBarrier.dstQueueFamilyIndex = dstScope.m_queueFamilyIndex;
    imageMemoryBarrier.image = vulkanImage->GetHandle();
    imageMemoryBarrier.subresourceRange = subresourceRange;
    m_imageMemoryBarriers.emplace_back(imageMemoryBarrier);
}

void VulkanPipelineBarrier::TransitionFullImage(VulkanRHLTexture *vulkanImage, ImageTransitionScope srcScope, ImageTransitionScope dstScope) {
    TransitionImage(vulkanImage, srcScope, dstScope, vulkanImage->GetDefaultImageView()->GetSubresourceRange());
}

vk::DependencyInfo VulkanPipelineBarrier::PackDependencyInfo() const {
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.imageMemoryBarrierCount = m_imageMemoryBarriers.size();
    dependencyInfo.pImageMemoryBarriers = m_imageMemoryBarriers.data();
    return dependencyInfo;
}