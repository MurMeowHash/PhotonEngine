#pragma once
#include <unordered_map>
#include "Shaders/VulkanShaderModule.h"

class VulkanVertexShadingStage {
    friend class VulkanGraphicsPipeline;
public:
    VulkanVertexShadingStage& SetVertexShader(VulkanShaderModule* shaderModule) {
        assert(shaderModule->GetStage() == vk::ShaderStageFlagBits::eVertex);
        m_shaderModule = shaderModule;
        return *this;
    }
private:
    VulkanShaderModule* m_shaderModule = nullptr;
};

class VulkanPrimitiveAssemblyStage {
    friend class VulkanGraphicsPipeline;
public:
    VulkanPrimitiveAssemblyStage& SetPrimitiveTopology(vk::PrimitiveTopology primitiveTopology) {
        m_primitiveTopology = primitiveTopology;
        return *this;
    }
private:
    vk::PrimitiveTopology m_primitiveTopology = vk::PrimitiveTopology::eTriangleList;
};

class VulkanRasterizationStage {
    friend class VulkanGraphicsPipeline;
public:
    VulkanRasterizationStage& SetPolygonMode(vk::PolygonMode polygonMode) {
        m_polygonMode = polygonMode;
        return *this;
    }

    VulkanRasterizationStage& SetCullMode(vk::CullModeFlags cullModeFlags) {
        m_cullModeFlags = cullModeFlags;
        return *this;
    }

private:
    vk::PolygonMode m_polygonMode = vk::PolygonMode::eFill;
    vk::CullModeFlags m_cullModeFlags = vk::CullModeFlagBits::eBack;
};

class VulkanFragmentShadingStage {
    friend class VulkanGraphicsPipeline;
public:
    VulkanFragmentShadingStage& SetVertexShader(VulkanShaderModule* shaderModule) {
        assert(shaderModule->GetStage() == vk::ShaderStageFlagBits::eFragment);
        m_shaderModule = shaderModule;
        return *this;
    }
private:
    VulkanShaderModule* m_shaderModule = nullptr;
};

class VulkanColorBlendStage {
    friend class VulkanGraphicsPipeline;
public:
    VulkanColorBlendStage& SetEnabled(bool enabled) {
        m_enabled = enabled;
        return *this;
    }
private:
    vk::Bool32 m_enabled = vk::False;
};

class VulkanPresentationStage {
    friend class VulkanGraphicsPipeline;
public:
    VulkanPresentationStage& AddColorAttachment(vk::Format colorAttachment) {
        m_colorAttachments.push_back(colorAttachment);
        return *this;
    }

    VulkanPresentationStage& AddDepthAttachment(vk::Format depthAttachment) {
        m_depthAttachment = depthAttachment;
        return *this;
    }
private:
    std::vector<vk::Format> m_colorAttachments;
    vk::Format m_depthAttachment = vk::Format::eUndefined;
};

class VulkanGraphicsPipelineDescriptor {
    friend class VulkanGraphicsPipeline;
public:
    VulkanGraphicsPipelineDescriptor& AddVertexStage(const VulkanVertexShadingStage& vertexShadingStage) {
        m_vertexShadingStage = vertexShadingStage;
        return *this;
    }

    VulkanGraphicsPipelineDescriptor& AddPrimitiveAssemblyStage(const VulkanPrimitiveAssemblyStage& primitiveAssemblyStage) {
        m_primitiveAssemblyStage = primitiveAssemblyStage;
        return *this;
    }

    VulkanGraphicsPipelineDescriptor& AddRasterizationStage(const VulkanRasterizationStage& rasterizationStage) {
        m_rasterizationStage = rasterizationStage;
        return *this;
    }

    VulkanGraphicsPipelineDescriptor& AddFragmentStage(const VulkanFragmentShadingStage& fragmentShadingStage) {
        m_fragmentShadingStage = fragmentShadingStage;
        return *this;
    }

    VulkanGraphicsPipelineDescriptor& AddColorBlendStage(const VulkanColorBlendStage& colorBlendStage) {
        m_colorBlendStage = colorBlendStage;
        return *this;
    }

    VulkanGraphicsPipelineDescriptor& AddPresentationStage(const VulkanPresentationStage& presentationStage) {
        m_presentationStage = presentationStage;
        return *this;
    }

    [[nodiscard]] size_t GetIdentifier() const {

    }

private:
    VulkanVertexShadingStage m_vertexShadingStage;
    VulkanPrimitiveAssemblyStage m_primitiveAssemblyStage;
    VulkanRasterizationStage m_rasterizationStage;
    VulkanFragmentShadingStage m_fragmentShadingStage;
    VulkanColorBlendStage m_colorBlendStage;
    VulkanPresentationStage m_presentationStage;
};