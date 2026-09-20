#include "../../Public/Pipelines/VulkanPipelineFactory.h"

VulkanGraphicsPipeline * Photon::Vulkan::PipelineFactory::CreateGraphicsPipeline(const VulkanGraphicsPipelineCreateInfo &createInfo, bool *isValid) {
    VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline();
    bool isCreated = pipeline->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return pipeline;
}

VulkanComputePipeline* Photon::Vulkan::PipelineFactory::CreateComputePipeline(const VulkanComputePipelineCreateInfo &createInfo, bool *isValid) {
    VulkanComputePipeline* pipeline = new VulkanComputePipeline();
    bool isCreated = pipeline->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return pipeline;
}