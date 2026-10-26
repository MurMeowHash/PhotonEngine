#pragma once

#include "PipelineState/IVulkanPipelineStateProperty.h"
#include <cstdint>
#include <unordered_map>
#include "VulkanDefinitions.h"

class VulkanRHLBuffer;
class VulkanGraphicsPipelineStateProperty;
class VulkanPipelineVertexInputState;

class VulkanVertexStreamPipelineStateProperty final : public IVulkanPipelineStateProperty {
public:
    void Invalidate() override;
    void Reset() override;
    void Record(VulkanPipelineStateRecordContext &recordContext) override;
public:
    void InjectProperties(VulkanGraphicsPipelineStateProperty* graphicsPipelineStateProperty);

    void SetVertexStream(uint32_t streamSource, VulkanRHLBuffer* vertexBuffer);

private:
    VulkanGraphicsPipelineStateProperty* m_graphicsPipelineStateProperty = nullptr;

    std::array<vk::Buffer, VULKAN_MAX_VERTEX_STREAMS> m_activeBufferHandles{};
    std::array<vk::DeviceSize, VULKAN_MAX_VERTEX_STREAMS> m_activeBufferOffsets{};
    uint32_t m_bindingsCount{};
    std::unordered_map<uint32_t, VulkanRHLBuffer*> m_streams;
};