#pragma once
#include "PipelineState/VulkanPipelineState.h"
#include "VulkanGraphicsPipelineStateBlueprint.h"
#include "PipelineState/GFX/VulkanViewportPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanScissorPipelineStateProperty.h"

class VulkanGraphicsPipelineState final : public VulkanPipelineState<VulkanGraphicsPipelineStateBlueprint> {
public:
    explicit VulkanGraphicsPipelineState();

    void SetViewport(const vk::Viewport& viewport);
    void SetScissor(const vk::Rect2D& scissor);

protected:
    void CommitInternal(VulkanCommandBuffer *commandBuffer) override;
    void InvalidatePropertiesDirtyFlag() override;

private:
    VulkanViewportPipelineStateProperty m_viewport;
    VulkanScissorPipelineStateProperty m_scissor;
};
