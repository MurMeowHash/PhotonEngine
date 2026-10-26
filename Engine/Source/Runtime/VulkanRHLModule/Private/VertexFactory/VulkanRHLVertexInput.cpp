#include "../../Public/VertexFactory/VulkanRHLVertexInput.h"

VulkanRHLVertexInput::VulkanRHLVertexInput(const VulkanRHLVertexInputInitializeInfo &createInfo)
: m_vertexLayout(createInfo.m_vertexLayout), m_vertexStreams(createInfo.m_vertexStreams) { }

const std::vector<VulkanRHLVertexStream> & VulkanRHLVertexInput::GetStreams() const {
    return m_vertexStreams;
}

VulkanRHLVertexLayout* VulkanRHLVertexInput::GetVertexLayout() const {
    return m_vertexLayout;
}