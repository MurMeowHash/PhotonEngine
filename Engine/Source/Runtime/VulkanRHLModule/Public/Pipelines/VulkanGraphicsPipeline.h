#pragma once
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanPipeline.h"
#include "Shaders/VulkanShaderDefinitions.h"
#include "Shaders/VulkanShaderPipeline.h"
#include "VulkanPipelineInternalState.h"

class VulkanDevice;
class VulkanRHLShader;
class VulkanRHLVertexLayout;

struct VulkanGraphicsPipelineCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanShaderPipeline m_shaderPipeline;
    VulkanRHLVertexLayout* m_vertexLayout;
    vk::PrimitiveTopology m_primitiveTopology;
    std::vector<vk::Format> m_colorAttachments;
};

class VulkanGraphicsPipeline : public VulkanPipeline {
public:
    ~VulkanGraphicsPipeline() override;
    static VulkanGraphicsPipeline* Create(const VulkanGraphicsPipelineCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    [[nodiscard]] VulkanPipelineVertexInputState* GetVertexInputState() const;

private:
    VulkanPipelineVertexInputState* m_vertexInputState = nullptr;
};
