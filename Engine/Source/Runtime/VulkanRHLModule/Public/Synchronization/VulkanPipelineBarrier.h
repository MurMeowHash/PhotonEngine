#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

class VulkanRHLTexture;

struct ImageTransitionScope {
    vk::ImageLayout m_layout;
    vk::PipelineStageFlags2 m_stage;
    vk::AccessFlags2 m_access;
    uint32_t m_queueFamilyIndex;
};

class VulkanPipelineBarrier {
public:
    void TransitionImage(VulkanRHLTexture* vulkanImage, ImageTransitionScope srcScope,
        ImageTransitionScope dstScope, vk::ImageSubresourceRange subresourceRange);
    void TransitionFullImage(VulkanRHLTexture* vulkanImage, ImageTransitionScope srcScope, ImageTransitionScope dstScope);

    [[nodiscard]] vk::DependencyInfo PackDependencyInfo() const;
private:
    std::vector<vk::ImageMemoryBarrier2> m_imageMemoryBarriers;
};