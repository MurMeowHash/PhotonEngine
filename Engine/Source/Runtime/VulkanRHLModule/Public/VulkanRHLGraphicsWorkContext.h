#pragma once

#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanRHLComputeWorkContext.h"
#include "Pipelines/VulkanRHLPipelineDescriptor.h"
#include "PipelineState/GFX/VulkanGraphicsPipelineState.h"
#include <glm/glm.hpp>
#include "VulkanGlobals.h"
#include "VertexFactory/VulkanRHLVertexInput.h"

struct VulkanRHLViewportRect {
    float m_minX;
    float m_minY;
    float m_minZ;
    float m_maxX;
    float m_maxY;
    float m_maxZ;
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

class VulkanRHLGraphicsWorkContext final : public VulkanRHLComputeWorkContext {
public:
    void Reset() override;

    [[nodiscard]] Photon::Result Execute() override;

public:
    static VulkanRHLGraphicsWorkContext* Create([[maybe_unused]] const VulkanRHLGraphicsWorkContextCreateInfo& createInfo,
                                                InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    void SetViewport(const VulkanRHLViewportRect& viewportRect);
    void SetScissor(const VulkanRHLRect& scissorRect);
    void BindPipeline(const VulkanRHLGraphicsPipelineDescriptor& pipelineDesc);
    void BindVertexInput(const VulkanRHLVertexInput& vertexInput);
    void DrawPrimitive(uint32_t vertexCount);
    void DrawMeshIndexed(VulkanRHLBuffer* indexBuffer, uint32_t indexCount);
    void BeginRenderPass(const VulkanRHLRenderPassBeginInfo& passBeginInfo);
    void EndRenderPass();

private:
    VulkanGraphicsPipelineState m_pendingGFXState;

    static vk::Rect2D RHLRectToVulkan(const VulkanRHLRect& rhlRect);
};