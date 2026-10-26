#pragma once

#include <cstdint>
#include "VulkanRHLVertexLayout.h"

class VulkanRHLBuffer;

struct VulkanRHLVertexStream {
    uint32_t m_streamSource;
    VulkanRHLBuffer* m_streamBuffer;
};

struct VulkanRHLVertexInputInitializeInfo {
    VulkanRHLVertexLayout* m_vertexLayout;
    std::vector<VulkanRHLVertexStream> m_vertexStreams;
};

class VulkanRHLVertexInput {
public:
    explicit VulkanRHLVertexInput(const VulkanRHLVertexInputInitializeInfo& createInfo);
    const std::vector<VulkanRHLVertexStream>& GetStreams() const;
private:
    VulkanRHLVertexLayout* m_vertexLayout = nullptr;
    std::vector<VulkanRHLVertexStream> m_vertexStreams;
};