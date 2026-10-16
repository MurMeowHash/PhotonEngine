#pragma once

#include "Resources/VulkanResourceTransition.h"

class VulkanRHLTexture;

struct VulkanTransitionInfo {
    VulkanTransitionInfo(const VulkanTransitionInfo& rhs) = delete;

    std::vector<vk::ImageMemoryBarrier2> m_imageMemoryBarriers;
    vk::DependencyInfo m_dependencyInfo;
    std::vector<VulkanRHLResource*> m_pendingResources;
};

class VulkanPipelineBarrier {
public:
    void TransitionImage(VulkanRHLTexture* vulkanImage, const VulkanPipelineImageTransitionScope& srcScope,
        const VulkanPipelineImageTransitionScope& destScope, vk::ImageSubresourceRange subresourceRange);

    [[nodiscard]] VulkanTransitionInfo PackTransitionInfo();
    void Reset();
private:
    std::vector<vk::ImageMemoryBarrier2> m_imageMemoryBarriers;
    std::vector<VulkanRHLResource*> m_pendingResources;

    void AddImageTransitionTracking(VulkanRHLTexture* rhlTexture, const VulkanPipelineImageTransitionScope& destScope);
};