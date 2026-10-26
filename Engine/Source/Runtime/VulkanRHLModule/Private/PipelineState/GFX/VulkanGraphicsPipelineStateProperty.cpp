#include "../../../Public/PipelineState/GFX/VulkanGraphicsPipelineStateProperty.h"
#include "CommandBuffers/VulkanCommandBuffer.h"
#include "Pipelines/VulkanGraphicsPipeline.h"

void VulkanGraphicsPipelineStateProperty::Record(VulkanPipelineStateRecordContext &recordContext) {
    if (m_isDirty) {
        recordContext.m_commandBuffer->GetHandle().bindPipeline(recordContext.m_pipelineBindPoint, m_property->GetHandle());
        m_isDirty = false;
    }
}

void VulkanGraphicsPipelineStateProperty::InjectProperties() { }