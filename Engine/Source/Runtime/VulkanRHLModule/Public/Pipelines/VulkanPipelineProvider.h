#pragma once
#include "VulkanGraphicsPipelineDescriptor.h"
#include "VulkanGraphicsPipeline.h"
#include <unordered_map>
#include "VulkanComputePipeline.h"

class VulkanPipelineProvider {
public:
    explicit VulkanPipelineProvider(VulkanDevice* vulkanDevice);
    ~VulkanPipelineProvider();

    VulkanPipeline* GetGraphicsPipeline(const VulkanGraphicsPipelineDescriptor& graphicsPipelineDesc);
    VulkanPipeline* GetComputePipeline(const VulkanComputePipelineDescriptor& computePipelineDesc);
private:
    VulkanDevice* m_vulkanDevice;
    std::unordered_map<size_t, VulkanPipeline*> m_pipelines;
};
