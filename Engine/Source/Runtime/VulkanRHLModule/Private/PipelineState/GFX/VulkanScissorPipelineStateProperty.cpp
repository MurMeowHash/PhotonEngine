#include "../../../Public/PipelineState/GFX/VulkanScissorPipelineStateProperty.h"
#include "CommandBuffers/VulkanCommandBuffer.h"

void VulkanScissorPipelineStateProperty::Record(VulkanPipelineStateRecordContext &recordContext) {
    if (m_isDirty) {
        recordContext.m_commandBuffer->GetHandle().setScissorWithCount(m_property);
        m_isDirty = false;
    }
}

void VulkanScissorPipelineStateProperty::InjectProperties() { }
