#pragma once

#include "PipelineState/VulkanPipelineStateSingleProperty.h"
#include <vulkan/vulkan.hpp>

class VulkanViewportPipelineStateProperty final : public VulkanPipelineStateSingleProperty<vk::Viewport> {
public:
    void Record(VulkanPipelineStateRecordContext &recordContext) override;
public:
    void InjectProperties();
};