#pragma once

#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanRHLComputeWorkContext.h"
#include "Pipelines/VulkanRHLPipelineDescriptor.h"
#include "PipelineState/GFX/VulkanGraphicsPipelineState.h"
#include <glm/glm.hpp>

struct VulkanRHLViewportRect {
    float m_minX;
    float m_minY;
    float m_minZ;
    float m_maxX;
    float m_maxY;
    float m_maxZ;
};

struct VulkanRHLRect {
    uint32_t m_minX;
    uint32_t m_minY;
    uint32_t m_maxX;
    uint32_t m_maxY;
};

struct VulkanRHLRenderPassColorAttachmentInput {
    VulkanRHLTexture* m_rhlTexture;
    glm::vec4 m_clearColor;
};

struct VulkanRHLRenderPassBeginInfo {
    std::vector<VulkanRHLRenderPassColorAttachmentInput> m_colorAttachments;
    VulkanRHLRect m_renderAffectedRect;
};

struct VulkanRHLGraphicsWorkContextCreateInfo {

};

class VulkanRHLGraphicsWorkContext : public VulkanRHLComputeWorkContext {
public:
    [[nodiscard]] Photon::Result Execute() override;

public:
    static VulkanRHLGraphicsWorkContext* Create([[maybe_unused]] const VulkanRHLGraphicsWorkContextCreateInfo& createInfo,
                                                InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    void SetViewport(const VulkanRHLViewportRect& viewportRect);
    void SetScissor(const VulkanRHLRect& scissorRect);
    void BindPipeline(const VulkanRHLGraphicsPipelineDescriptor& pipelineDesc);
    void DrawPrimitive(uint32_t vertexCount);
    void BeginRenderPass(const VulkanRHLRenderPassBeginInfo& passBeginInfo);
    void EndRenderPass();

private:
    VulkanGraphicsPipelineState m_pendingGFXState;

    static vk::Rect2D RHLRectToVulkan(const VulkanRHLRect& rhlRect);
};