#include "../../Public/Shaders/VulkanShaderPipeline.h"

#define ADD_SHADER_STAGE_INFO_SAFE(shaderStageCreateInfo, rhlShader)                        \
    if(rhlShader != nullptr)                                                                \
        shaderStageCreateInfo.emplace_back(GetShaderTagetPipelineInfo(rhlShader));

VulkanShaderPipeline::VulkanShaderPipeline(const VulkanShaderPipelineCreateInfo &createInfo)
: m_vertexShader(createInfo.m_vertexShader), m_fragmentShader(createInfo.m_fragmentShader) {
    GatherShaderStageData();
    GatherVertexAttributeData();
}

const std::vector<vk::PipelineShaderStageCreateInfo>& VulkanShaderPipeline::GetShaderStageCreateInfo() const {
    return m_shaderStageCreateInfo;
}

const std::unordered_map<VulkanRHLShaderSemantic, VulkanShaderVertexAttribute>& VulkanShaderPipeline::GetVertexAttributes() const {
    return m_filteredVertexAttributes;
}

bool VulkanShaderPipeline::ValidateVertexAttribute(VulkanRHLShaderSemantic semantic, VulkanRHLFormat vertexInputFormat) const {
    auto vertexAttributeIterator = m_filteredVertexAttributes.find(semantic);
    if (vertexAttributeIterator == m_filteredVertexAttributes.end())
        return false;

    return IsRHLFormatCompatibleWithShader(vertexInputFormat, vertexAttributeIterator->second.m_format);
}

uint32_t VulkanShaderPipeline::GetVertexAttributeLocation(VulkanRHLShaderSemantic semantic) const {
    auto vertexAttributeIterator = m_filteredVertexAttributes.find(semantic);
    if (vertexAttributeIterator == m_filteredVertexAttributes.end())
        return 0u;

    return vertexAttributeIterator->second.m_location;
}

void VulkanShaderPipeline::GatherShaderStageData() {
    m_shaderStageCreateInfo.clear();
    m_shaderStageCreateInfo.reserve(ShadersCount);

    ADD_SHADER_STAGE_INFO_SAFE(m_shaderStageCreateInfo, m_vertexShader)
    ADD_SHADER_STAGE_INFO_SAFE(m_shaderStageCreateInfo, m_fragmentShader)
}

void VulkanShaderPipeline::GatherVertexAttributeData() {
    assert(m_vertexShader != nullptr);

    m_filteredVertexAttributes.reserve(m_vertexShader->GetMetaData().m_vertexAttributes.size());
    for (const VulkanShaderVertexAttribute& vertexAttribute : m_vertexShader->GetMetaData().m_vertexAttributes) {
        if (!vertexAttribute.m_semantic.IsValid())
            continue;

        m_filteredVertexAttributes.emplace(vertexAttribute.m_semantic, vertexAttribute);
    }
}

vk::PipelineShaderStageCreateInfo VulkanShaderPipeline::GetShaderTagetPipelineInfo(VulkanRHLShader *rhlShader) {
    return vk::PipelineShaderStageCreateInfo({}, rhlShader->GetVulkanShaderStage(), rhlShader->GetHandle(), rhlShader->GetEntryName());
}

bool VulkanShaderPipeline::IsRHLFormatCompatibleWithShader(VulkanRHLFormat rhlFormat, VulkanRHLShaderFormat shaderFormat) {
    switch (shaderFormat) {
        case VulkanRHLShaderFormat::R32G32_Float:
            return rhlFormat == VulkanRHLFormat::R32G32_Float;
        case VulkanRHLShaderFormat::R32G32B32_Float:
            return rhlFormat == VulkanRHLFormat::R32G32B32_Float;
        default:
            return false;
    }
}