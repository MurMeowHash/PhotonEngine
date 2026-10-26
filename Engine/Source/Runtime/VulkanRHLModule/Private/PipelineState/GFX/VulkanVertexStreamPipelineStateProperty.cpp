#include "../../../Public/PipelineState/GFX/VulkanVertexStreamPipelineStateProperty.h"
#include "CommandBuffers/VulkanCommandBuffer.h"
#include "Pipelines/VulkanGraphicsPipeline.h"
#include "PipelineState/GFX/VulkanGraphicsPipelineStateProperty.h"
#include "Resources/VulkanBuffer.h"
#include "Resources/VulkanRHLBuffer.h"

void VulkanVertexStreamPipelineStateProperty::Invalidate() {
    m_bindingsCount = 0;
}

void VulkanVertexStreamPipelineStateProperty::Reset() {
    m_streams.clear();
}

void VulkanVertexStreamPipelineStateProperty::Record(VulkanPipelineStateRecordContext &recordContext) {
    if (m_streams.empty())
        return;

    VulkanPipelineVertexInputState* activeVertexInputState = m_graphicsPipelineStateProperty->GetProperty()->GetVertexInputState();
    const uint32_t bindingsCount = activeVertexInputState->GetBindingsCount();

    std::array<vk::Buffer, VULKAN_MAX_VERTEX_STREAMS> bufferHandles{nullptr};
    std::array<vk::DeviceSize, VULKAN_MAX_VERTEX_STREAMS> bufferOffsets{0};
    for (uint32_t bindingIndex = 0; bindingIndex < bindingsCount; bindingIndex++) {
        uint32_t streamSource = activeVertexInputState->GetStreamForBinding(bindingIndex);
        auto streamIterator = m_streams.find(streamSource);
        if (streamIterator == m_streams.end())
            return;

        VulkanBufferSlice bufferHandle = streamIterator->second->GetHandle();
        bufferHandles[bindingIndex] = bufferHandle.GetBuffer()->GetHandle();
        bufferOffsets[bindingIndex] = bufferHandle.GetOffset();
    }

    if (m_bindingsCount == bindingsCount
        && std::equal(bufferHandles.begin(), bufferHandles.begin() + bindingsCount, m_activeBufferHandles.begin(), m_activeBufferHandles.begin() + m_bindingsCount)
        && std::equal(bufferOffsets.begin(), bufferOffsets.begin() + bindingsCount, m_activeBufferOffsets.begin(), m_activeBufferOffsets.begin() + m_bindingsCount))
        return;

    recordContext.m_commandBuffer->GetHandle().bindVertexBuffers(0, bindingsCount, bufferHandles.data(), bufferOffsets.data());

    m_activeBufferHandles = bufferHandles;
    m_activeBufferOffsets = bufferOffsets;
    m_bindingsCount = bindingsCount;
}

void VulkanVertexStreamPipelineStateProperty::InjectProperties(VulkanGraphicsPipelineStateProperty *graphicsPipelineStateProperty) {
    m_graphicsPipelineStateProperty = graphicsPipelineStateProperty;
}

void VulkanVertexStreamPipelineStateProperty::SetVertexStream(uint32_t streamSource, VulkanRHLBuffer *vertexBuffer) {
    m_streams[streamSource] = vertexBuffer;
}