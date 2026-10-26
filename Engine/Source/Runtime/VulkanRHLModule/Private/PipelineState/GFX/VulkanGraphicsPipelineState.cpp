#include "../Public/PipelineState/GFX/VulkanGraphicsPipelineState.h"

VulkanGraphicsPipelineState::VulkanGraphicsPipelineState() {
    m_pipelineBlueprint.Create(&m_pipeline, &m_viewport, &m_scissor, &m_vertexStream);
}

void VulkanGraphicsPipelineState::SetGraphicsPipeline(VulkanGraphicsPipeline *graphicsPipeline) {
    m_pipeline.SetProperty(graphicsPipeline);
}

void VulkanGraphicsPipelineState::SetViewport(const vk::Viewport &viewport) {
    m_viewport.SetProperty(viewport);
}

void VulkanGraphicsPipelineState::SetScissor(const vk::Rect2D &scissor) {
    m_scissor.SetProperty(scissor);
}

void VulkanGraphicsPipelineState::SetVertexStream(uint32_t streamSource, VulkanRHLBuffer *vertexBuffer) {
    m_vertexStream.SetVertexStream(streamSource, vertexBuffer);
}

void VulkanGraphicsPipelineState::Reset() {
    m_pipeline.Reset();
    m_viewport.Reset();
    m_scissor.Reset();
    m_vertexStream.Reset();
}

void VulkanGraphicsPipelineState::CommitInternal(VulkanCommandBuffer *commandBuffer) {
    VulkanPipelineStateRecordContext recordContext{};
    recordContext.m_commandBuffer = commandBuffer;
    recordContext.m_pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    m_pipeline.Record(recordContext);
    m_viewport.Record(recordContext);
    m_scissor.Record(recordContext);
    m_vertexStream.Record(recordContext);
}

void VulkanGraphicsPipelineState::InvalidatePropertiesDirtyFlag() {
    m_pipeline.Invalidate();
    m_viewport.Invalidate();
    m_scissor.Invalidate();
    m_vertexStream.Invalidate();
}