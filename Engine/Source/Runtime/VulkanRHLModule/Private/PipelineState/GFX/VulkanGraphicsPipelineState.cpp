#include "../Public/PipelineState/GFX/VulkanGraphicsPipelineState.h"

VulkanGraphicsPipelineState::VulkanGraphicsPipelineState() {
    m_pipelineBlueprint.Create(&m_pipeline, &m_viewport, &m_scissor);
}

void VulkanGraphicsPipelineState::SetViewport(const vk::Viewport &viewport) {
    m_viewport.SetProperty(viewport);
}

void VulkanGraphicsPipelineState::SetScissor(const vk::Rect2D &scissor) {
    m_scissor.SetProperty(scissor);
}

void VulkanGraphicsPipelineState::CommitInternal(VulkanCommandBuffer *commandBuffer) {
    VulkanPipelineStateRecordContext recordContext{};
    recordContext.m_commandBuffer = commandBuffer;
    recordContext.m_pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    m_pipeline.Record(recordContext);
    m_viewport.Record(recordContext);
    m_scissor.Record(recordContext);
}

void VulkanGraphicsPipelineState::InvalidatePropertiesDirtyFlag() {
    m_pipeline.Invalidate();
    m_viewport.Invalidate();
    m_scissor.Invalidate();
}