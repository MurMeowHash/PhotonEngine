#pragma once
#include "PipelineState/VulkanPipelineState.h"
#include "VulkanGraphicsPipelineStateBlueprint.h"
#include "PipelineState/GFX/VulkanViewportPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanScissorPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanGraphicsPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanVertexStreamPipelineStateProperty.h"

class VulkanGraphicsPipelineState final : public VulkanPipelineState<VulkanGraphicsPipelineStateBlueprint> {
public:
    void Reset() override;
protected:
    void CommitInternal(VulkanCommandBuffer *commandBuffer) override;
    void InvalidatePropertiesDirtyFlag() override;

public:
    explicit VulkanGraphicsPipelineState();

    void SetGraphicsPipeline(VulkanGraphicsPipeline* graphicsPipeline);
    void SetViewport(const vk::Viewport& viewport);
    void SetScissor(const vk::Rect2D& scissor);
    void SetVertexStream(uint32_t streamSource, VulkanRHLBuffer* vertexBuffer);

private:
    VulkanGraphicsPipelineStateProperty m_pipeline;
    VulkanViewportPipelineStateProperty m_viewport;
    VulkanScissorPipelineStateProperty m_scissor;
    VulkanVertexStreamPipelineStateProperty m_vertexStream;
};
