#pragma once

#include "PipelineState/VulkanPipelineStateSingleProperty.h"
#include <vulkan/vulkan.hpp>

class VulkanScissorPipelineStateProperty final : public VulkanPipelineStateSingleProperty<vk::Rect2D> {
public:
    void Record(VulkanPipelineStateRecordContext &recordContext) override;
public:
    void InjectProperties();
};