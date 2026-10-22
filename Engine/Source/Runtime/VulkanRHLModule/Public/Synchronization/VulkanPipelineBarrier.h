#pragma once

#include "Resources/VulkanResourceTransition.h"

class VulkanRHLTexture;
class VulkanBuffer;

struct VulkanTransitionInfo {
    std::vector<vk::ImageMemoryBarrier2> m_imageMemoryBarriers;
    std::vector<vk::BufferMemoryBarrier2> m_bufferMemoryBarriers;
    vk::DependencyInfo m_dependencyInfo;
    std::vector<VulkanResource*> m_pendingResources;
};

class VulkanPipelineBarrier {
public:
    void TransitionImage(VulkanRHLTexture* vulkanImage, const VulkanPipelineImageTransitionScope& srcScope,
        const VulkanPipelineImageTransitionScope& destScope, vk::ImageSubresourceRange subresourceRange);
    void TransitionBuffer(VulkanBuffer* vulkanBuffer, const VulkanPipelineBufferTransitionScope& srcScope,
        const VulkanPipelineBufferTransitionScope& destScope, vk::DeviceSize offset, vk::DeviceSize size);

    [[nodiscard]] VulkanTransitionInfo PackTransitionInfo();
    void Reset();
private:
    std::vector<vk::ImageMemoryBarrier2> m_imageMemoryBarriers;
    std::vector<vk::BufferMemoryBarrier2> m_bufferMemoryBarriers;
    std::vector<VulkanResource*> m_pendingResources;

    void AddImageTransitionTracking(VulkanRHLTexture* rhlTexture, const VulkanPipelineImageTransitionScope& destScope);
    void AddBufferTransitionTracking(VulkanBuffer* buffer, const VulkanPipelineBufferTransitionScope& destScope);
};