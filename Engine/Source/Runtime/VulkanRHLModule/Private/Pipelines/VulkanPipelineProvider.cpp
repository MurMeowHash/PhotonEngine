#include "../../Public/Pipelines/VulkanPipelineProvider.h"
#include <ranges>
#include "Pipelines/VulkanPipelineFactory.h"

VulkanPipelineProvider::VulkanPipelineProvider(VulkanDevice *vulkanDevice)
: m_vulkanDevice(vulkanDevice) { }

VulkanPipelineProvider::~VulkanPipelineProvider() {
    for (VulkanPipeline *&pipeline: m_pipelines | std::views::values) {
        delete pipeline;
    }
}

VulkanPipeline* VulkanPipelineProvider::GetGraphicsPipeline(const VulkanGraphicsPipelineDescriptor &graphicsPipelineDesc) {
    size_t pipelineDescIdentifier = graphicsPipelineDesc.GetIdentifier();
    auto pipelineIterator = m_pipelines.find(pipelineDescIdentifier);
    if (pipelineIterator != m_pipelines.end())
        return pipelineIterator->second;

    const VulkanGraphicsPipelineCreateInfo createInfo(m_vulkanDevice, graphicsPipelineDesc);
    VulkanPipeline* graphicsPipeline = Photon::Vulkan::PipelineFactory::CreateGraphicsPipeline(createInfo);
    m_pipelines.emplace(pipelineDescIdentifier, graphicsPipeline);
    return graphicsPipeline;
}

VulkanPipeline* VulkanPipelineProvider::GetComputePipeline(const VulkanComputePipelineDescriptor& computePipelineDesc) {
    size_t pipelineDescIdentifier = computePipelineDesc.GetIdentifier();
    auto pipelineIterator = m_pipelines.find(pipelineDescIdentifier);
    if (pipelineIterator != m_pipelines.end())
        return pipelineIterator->second;

    const VulkanComputePipelineCreateInfo createInfo(m_vulkanDevice, computePipelineDesc);
    VulkanPipeline* graphicsPipeline = Photon::Vulkan::PipelineFactory::CreateComputePipeline(createInfo);
    m_pipelines.emplace(pipelineDescIdentifier, graphicsPipeline);
    return graphicsPipeline;
}