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
}

vk::DependencyInfo VulkanPipelineBarrier::PackDependencyInfo() const {
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.imageMemoryBarrierCount = m_imageMemoryBarriers.size();
    dependencyInfo.pImageMemoryBarriers = m_imageMemoryBarriers.data();
    return dependencyInfo;
}
