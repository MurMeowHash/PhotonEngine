#include "../../../Public/PipelineState/GFX/VulkanViewportPipelineStateProperty.h"
#include "CommandBuffers/VulkanCommandBuffer.h"

void VulkanViewportPipelineStateProperty::Record(VulkanPipelineStateRecordContext &recordContext) {
    if (m_isDirty) {
        recordContext.m_commandBuffer->GetHandle().setViewportWithCount(m_property);
        m_isDirty = false;
    }
}

void VulkanViewportPipelineStateProperty::InjectProperties() { }
