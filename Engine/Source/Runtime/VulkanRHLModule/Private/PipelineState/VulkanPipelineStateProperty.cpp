#include "../../Public/PipelineState/VulkanPipelineStateProperty.h"
#include "CommandBuffers/VulkanCommandBuffer.h"
#include "Pipelines/VulkanPipeline.h"

void VulkanPipelineStateProperty::Record(VulkanPipelineStateRecordContext &recordContext) {
    if (m_isDirty) {
        recordContext.m_commandBuffer->GetHandle().bindPipeline(recordContext.m_pipelineBindPoint, m_property->GetHandle());
        m_isDirty = false;
    }
}

void VulkanPipelineStateProperty::InjectProperties() { }
