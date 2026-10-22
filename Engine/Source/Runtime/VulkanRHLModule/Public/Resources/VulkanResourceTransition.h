#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanGlobals.h"
#include <memory>

class VulkanResource;
class VulkanRHLTexture;
class VulkanDevice;
class VulkanQueue;
class VulkanBinarySemaphore;

using Photon::Vulkan::RHLPipelineUsage;
using Photon::Vulkan::RHLPipeline;

struct VulkanPipelineResourceTransitionScope {
    vk::PipelineStageFlags2 m_stage;
    vk::AccessFlags2 m_access;
    uint32_t m_queueFamily;
};

struct VulkanPipelineImageTransitionScope : public VulkanPipelineResourceTransitionScope {
    vk::ImageLayout m_layout;
};

struct VulkanPipelineBufferTransitionScope : public VulkanPipelineResourceTransitionScope {

};

struct VulkanRHLTextureTransitionRequest {
    VulkanRHLTexture* m_rhlTexture;
    RHLPipelineUsage m_srcUsage;
    RHLPipelineUsage m_destUsage;
    //Maybe subresources??
};

class VulkanRHLResourceTransitionRequestBatch {
public:
    VulkanRHLResourceTransitionRequestBatch(RHLPipeline srcPipeline, RHLPipeline destPipeline);

    void AddTextureTransitionRequest(VulkanRHLTextureTransitionRequest&& textureRequest);
    [[nodiscard]] const std::vector<VulkanRHLTextureTransitionRequest>& GetTextureTransitionRequests() const;
    [[nodiscard]] RHLPipeline GetSrcPipeline() const;
    [[nodiscard]] RHLPipeline GetDestPipeline() const;
private:
    std::vector<VulkanRHLTextureTransitionRequest> m_textureTransitionRequests;
    RHLPipeline m_srcPipeline{};
    RHLPipeline m_destPipeline{};
};

struct VulkanResourceTransitionScope {
    vk::PipelineStageFlags2 m_stage;
    vk::AccessFlags2 m_access;
};

struct VulkanImageTransitionScope : VulkanResourceTransitionScope {
    vk::ImageLayout m_layout;
};

struct VulkanImagePendingTransition {
    VulkanRHLTexture* m_rhlTexture;
    VulkanImageTransitionScope m_srcScope;
    VulkanImageTransitionScope m_destScope;
    vk::ImageSubresourceRange m_subresourceRange;
};

class VulkanRHLPendingTransition {
public:
    VulkanRHLPendingTransition(VulkanDevice* vulkanDevice, RHLPipeline srcPipeline, RHLPipeline destPipeline);
    void CreateTexturePendingTransition(const VulkanRHLTextureTransitionRequest& textureRequest);
    [[nodiscard]] const std::vector<VulkanImagePendingTransition>& GetImagePendingTransitions() const;
    [[nodiscard]] RHLPipeline GetSrcPipeline() const;
    [[nodiscard]] RHLPipeline GetDestPipeline() const;
    [[nodiscard]] uint32_t GetSrcQueueFamily() const;
    [[nodiscard]] uint32_t GetDestQueueFamily() const;
    [[nodiscard]] bool IsCrossPipeline() const;
    [[nodiscard]] std::shared_ptr<VulkanBinarySemaphore> GetTransitionSemaphore() const;
private:
    VulkanDevice* m_vulkanDevice;

    std::vector<VulkanImagePendingTransition> m_imagePendingTransitions;
    RHLPipeline m_srcPipeline{};
    RHLPipeline m_destPipeline{};
    VulkanQueue* m_srcVulkanQueue = nullptr;
    VulkanQueue* m_destVulkanQueue = nullptr;
    std::shared_ptr<VulkanBinarySemaphore> m_transitionSemaphore = nullptr;

    static void FillBasicResourceTransitionScope(RHLPipelineUsage usage, VulkanResourceTransitionScope& transitionScope);
    static void FillBasicSrcResourceTransitionScope(VulkanResource* rhlResource, RHLPipelineUsage usage, VulkanResourceTransitionScope& transitionScope);
    static void FillBasicDestResourceTransitionScope(RHLPipelineUsage usage, VulkanResourceTransitionScope& transitionScope);
    static void GetTransitionImageLayout(RHLPipelineUsage usage, vk::ImageLayout& layout);
    static void GetSrcTransitionImageLayout(VulkanRHLTexture* rhlTexture, RHLPipelineUsage usage, vk::ImageLayout& layout);
    static void GetDestTransitionImageLayout(RHLPipelineUsage usage, vk::ImageLayout& layout);
    [[nodiscard]] static VulkanImageTransitionScope CreateSrcImageTransitionScope(VulkanRHLTexture* rhlTexture, RHLPipelineUsage usage);
    [[nodiscard]] static VulkanImageTransitionScope CreateDestImageTransitionScope(RHLPipelineUsage usage);

    [[nodiscard]] VulkanQueue* GetVulkanQueueByRHLPipeline(RHLPipeline pipeline) const;
};