#pragma once
#include "PipelineState/VulkanPipelineStateBlueprint.h"

class VulkanGraphicsPipelineStateBlueprint : public VulkanPipelineStateBlueprint {
protected:
    void InstallProperties() override;
};