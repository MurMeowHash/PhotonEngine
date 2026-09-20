#include "../../Public/Pipelines/VulkanComputePipeline.h"
#include "Device/VulkanDevice.h"

bool VulkanComputePipeline::Create(const VulkanComputePipelineCreateInfo &createInfo) {
    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo;
    VulkanShaderModule* computeShader = createInfo.m_pipelineDesc.m_computeShaderModule;
    shaderStageCreateInfo.module = computeShader->GetHandle();
    shaderStageCreateInfo.pName = computeShader->GetEntryName();
    assert(computeShader->GetStage() == vk::ShaderStageFlagBits::eCompute);
    shaderStageCreateInfo.stage = computeShader->GetStage();

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    vk::ResultValue<vk::raii::PipelineLayout> pipelineLayoutWrapper = createInfo.m_vulkanDevice->GetHandle().createPipelineLayout(pipelineLayoutCreateInfo);
    if (pipelineLayoutWrapper.result != vk::Result::eSuccess)
        return false;

    vk::ComputePipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.stage = shaderStageCreateInfo;
    pipelineCreateInfo.layout = pipelineLayoutWrapper.value;
    vk::ResultValue<vk::raii::Pipeline> pipelineWrapper = createInfo.m_vulkanDevice->GetHandle().createComputePipeline(nullptr, pipelineCreateInfo);
    if (pipelineWrapper.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(pipelineWrapper.value);
    return true;
}