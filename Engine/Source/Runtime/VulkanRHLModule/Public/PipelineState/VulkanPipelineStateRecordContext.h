#pragma once

#include <vulkan/vulkan.hpp>

class VulkanCommandBuffer;

struct VulkanPipelineStateRecordContext {
    VulkanCommandBuffer* m_commandBuffer;
    vk::PipelineBindPoint m_pipelineBindPoint;
};