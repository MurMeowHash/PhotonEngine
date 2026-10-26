#pragma once
#include "PipelineState/VulkanPipelineStateSingleProperty.h"

class VulkanGraphicsPipeline;

class VulkanGraphicsPipelineStateProperty : public VulkanPipelineStateSingleProperty<VulkanGraphicsPipeline*> {
public:
    void Record(VulkanPipelineStateRecordContext &recordContext) override;

public:
    void InjectProperties();
};