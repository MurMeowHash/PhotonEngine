#pragma once
#include "PipelineState/VulkanPipelineState.h"
#include "VulkanGraphicsPipelineStateBlueprint.h"
#include "PipelineState/GFX/VulkanViewportPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanScissorPipelineStateProperty.h"

class VulkanGraphicsPipelineState final : public VulkanPipelineState<VulkanGraphicsPipelineStateBlueprint> {
public:
    void Reset() override;
protected:
    void CommitInternal(VulkanCommandBuffer *commandBuffer) override;
    void InvalidatePropertiesDirtyFlag() override;

public:
    explicit VulkanGraphicsPipelineState();

    void SetViewport(const vk::Viewport& viewport);
    void SetScissor(const vk::Rect2D& scissor);

private:
    VulkanViewportPipelineStateProperty m_viewport;
    VulkanScissorPipelineStateProperty m_scissor;
};
