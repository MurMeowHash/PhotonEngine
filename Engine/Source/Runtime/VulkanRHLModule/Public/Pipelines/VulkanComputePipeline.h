#pragma once
#include "VulkanComputePipelineDescriptor.h"
#include "VulkanPipeline.h"

class VulkanDevice;

struct VulkanComputePipelineCreateInfo {
    VulkanDevice* m_vulkanDevice;
    const VulkanComputePipelineDescriptor& m_pipelineDesc;
};

class VulkanComputePipeline : public VulkanPipeline {
public:
    [[nodiscard]] bool Create(const VulkanComputePipelineCreateInfo& createInfo);
};
