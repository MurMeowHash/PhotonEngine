#pragma once
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanPipeline.h"

class VulkanDevice;
class VulkanRHLShader;

struct VulkanGraphicsPipelineCreateInfo {
    VulkanDevice* m_vulkanDevice;
    std::vector<VulkanRHLShader*> m_shaders;
    vk::PrimitiveTopology m_primitiveTopology;
    std::vector<vk::Format> m_colorAttachments;
};

class VulkanGraphicsPipeline : public VulkanPipeline {
public:
    static VulkanGraphicsPipeline* Create(const VulkanGraphicsPipelineCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
private:
    static std::vector<vk::PipelineShaderStageCreateInfo> AssembleShaderPipelineData(const std::vector<VulkanRHLShader*>& shaders);
};
