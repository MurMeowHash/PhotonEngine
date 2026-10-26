#pragma once

#include <vector>
#include <unordered_map>
#include "VulkanRHLShader.h"
#include "VulkanDefinitions.h"

struct VulkanShaderPipelineCreateInfo {
    VulkanRHLVertexShader* m_vertexShader;
    VulkanRHLFragmentShader* m_fragmentShader;
};

class VulkanShaderPipeline {
public:
    VulkanShaderPipeline() = default;
    explicit VulkanShaderPipeline(const VulkanShaderPipelineCreateInfo& createInfo);
    [[nodiscard]] const std::vector<vk::PipelineShaderStageCreateInfo>& GetShaderStageCreateInfo() const;
    [[nodiscard]] bool ValidateVertexAttribute(VulkanRHLShaderSemantic semantic, VulkanRHLFormat vertexInputFormat) const;
    [[nodiscard]] uint32_t GetVertexAttributeLocation(VulkanRHLShaderSemantic semantic) const;
    [[nodiscard]] const std::unordered_map<VulkanRHLShaderSemantic, VulkanShaderVertexAttribute>& GetVertexAttributes() const;
private:
    enum { ShadersCount = 2 };

    VulkanRHLVertexShader* m_vertexShader = nullptr;
    VulkanRHLFragmentShader* m_fragmentShader = nullptr;

    std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStageCreateInfo{};
    std::unordered_map<VulkanRHLShaderSemantic, VulkanShaderVertexAttribute> m_filteredVertexAttributes;

    void GatherShaderStageData();
    void GatherVertexAttributeData();

    static vk::PipelineShaderStageCreateInfo GetShaderTagetPipelineInfo(VulkanRHLShader* rhlShader);
    static bool IsRHLFormatCompatibleWithShader(VulkanRHLFormat rhlFormat, VulkanRHLShaderFormat shaderFormat);
};