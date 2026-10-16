#pragma once

#include "VulkanPipelineStateRecordContext.h"

class IVulkanPipelineStateProperty {
public:
    virtual ~IVulkanPipelineStateProperty() = default;
    virtual void Invalidate() = 0;
    virtual void Record(VulkanPipelineStateRecordContext& recordContext) = 0;
};