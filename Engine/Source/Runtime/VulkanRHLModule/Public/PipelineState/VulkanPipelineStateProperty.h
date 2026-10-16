#pragma once

#include "VulkanPipelineStateSingleProperty.h"

class VulkanPipeline;

class VulkanPipelineStateProperty final : public VulkanPipelineStateSingleProperty<VulkanPipeline*> {
public:
    void Record(VulkanPipelineStateRecordContext& recordContext) override;
public:
    void InjectProperties();
};