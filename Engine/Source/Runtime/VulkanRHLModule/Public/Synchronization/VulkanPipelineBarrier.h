#pragma once

#include "Resources/VulkanResourceTransition.h"

class VulkanRHLTexture;

struct VulkanPipelineImageTransitionScope {
    vk::PipelineStageFlags2 m_stage;
    vk::AccessFlags2 m_access;
    vk::ImageLayout m_layout;
    uint32_t m_queueFamily;
};

class VulkanPipelineBarrier {
public:
    void TransitionImage(VulkanRHLTexture* vulkanImage, const VulkanPipelineImageTransitionScope& srcScope,
        const VulkanPipelineImageTransitionScope& destScope, vk::ImageSubresourceRange subresourceRange);

    [[nodiscard]] vk::DependencyInfo PackDependencyInfo() const;
private:
    std::vector<vk::ImageMemoryBarrier2> m_imageMemoryBarriers;
};