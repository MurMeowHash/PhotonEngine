#pragma once
#include "VulkanGraphicsPipelineDescriptor.h"
#include "VulkanPipeline.h"

class VulkanDevice;

struct VulkanGraphicsPipelineCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanGraphicsPipelineDescriptor m_pipelineDesc;
};

class VulkanGraphicsPipeline : public VulkanPipeline {
public:
    [[nodiscard]] bool Create(const VulkanGraphicsPipelineCreateInfo& createInfo);
private:
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;

    [[nodiscard]] bool CreatePipelineLayout(const vk::raii::Device& device);

    static std::vector<vk::PipelineShaderStageCreateInfo> AssembleShaderPipelineData(const VulkanGraphicsPipelineDescriptor& pipelineDesc);
    static void AddShaderToAssembleData(VulkanShaderModule* vulkanShader, std::vector<vk::PipelineShaderStageCreateInfo>& assembledShaders);
};
