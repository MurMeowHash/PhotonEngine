#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanGlobals.h"
#include <memory>

class VulkanRHLResource;
class VulkanRHLTexture;
class VulkanDevice;
class VulkanQueue;
class VulkanBinarySemaphore;

using Photon::Vulkan::RHLPipelineUsage;
using Photon::Vulkan::RHLWorkType;

struct VulkanRHLTextureTransitionRequest {
    VulkanRHLTexture* m_rhlTexture;
    //Maybe subresources??
};

class VulkanRHLResourceTransitionRequestBatch {
public:
    VulkanRHLResourceTransitionRequestBatch(RHLWorkType srcWorkType, RHLWorkType destWorkType);

    void AddTextureTransitionRequest(VulkanRHLTextureTransitionRequest&& textureRequest);
    [[nodiscard]] const std::vector<VulkanRHLTextureTransitionRequest>& GetTextureTransitionRequests() const;
    [[nodiscard]] RHLWorkType GetSrcWorkType() const;
    [[nodiscard]] RHLWorkType GetDestWorkType() const;
private:
    std::vector<VulkanRHLTextureTransitionRequest> m_textureTransitionRequests;
    RHLWorkType m_srcWorkType{};
    RHLWorkType m_destWorkType{};
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
    VulkanRHLPendingTransition(VulkanDevice* vulkanDevice, RHLWorkType srcWorkType, RHLWorkType destWorkType);
    void CreateTexturePendingTransition(const VulkanRHLTextureTransitionRequest& textureRequest);
    [[nodiscard]] const std::vector<VulkanImagePendingTransition>& GetImagePendingTransitions() const;
    [[nodiscard]] RHLWorkType GetSrcWorkType() const;
    [[nodiscard]] RHLWorkType GetDestWorkType() const;
    [[nodiscard]] uint32_t GetSrcQueueFamily() const;
    [[nodiscard]] uint32_t GetDestQueueFamily() const;
    [[nodiscard]] bool IsCrossQueue() const;
    [[nodiscard]] std::shared_ptr<VulkanBinarySemaphore> GetTransitionSemaphore() const;
private:
    VulkanDevice* m_vulkanDevice;

    std::vector<VulkanImagePendingTransition> m_imagePendingTransitions;
    RHLWorkType m_srcWorkType{};
    RHLWorkType m_destWorkType{};
    VulkanQueue* m_srcVulkanQueue = nullptr;
    VulkanQueue* m_destVulkanQueue = nullptr;
    std::shared_ptr<VulkanBinarySemaphore> m_transitionSemaphore = nullptr;

    static void FillBasicResourceTransitionScope(VulkanResourceTransitionScope& transitionScope, RHLPipelineUsage usage);
    static void GetTransitionImageLayout(RHLPipelineUsage usage, vk::ImageLayout& layout);
    [[nodiscard]] static VulkanImageTransitionScope CreateImageTransitionScope(RHLPipelineUsage usage);

    [[nodiscard]] VulkanQueue* GetVulkanQueueByRHLWork(RHLWorkType workType) const;
};