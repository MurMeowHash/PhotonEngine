#include "../../Public/Pipelines/VulkanPipelineInternalState.h"
#include "VulkanUtils.h"
#include <algorithm>
#include <unordered_set>
#include <utility>

VulkanPipelineVertexInputState* VulkanPipelineVertexInputState::Create(const VulkanPipelineVertexInputStateCreateInfo &createInfo,
                                                                       InOutCreateParams<Photon::Result> *inOutCreateParams) {
    const std::vector<VulkanRHLVertexDescription>& layoutVertexDescriptions = createInfo.m_vertexLayout->GetVertexDescriptions();
    const std::unordered_map<VulkanRHLShaderSemantic, VulkanShaderVertexAttribute>& shaderVertexAttributes =
        createInfo.m_shaderPipeline.GetVertexAttributes();

    std::vector<std::pair<const VulkanShaderVertexAttribute*, const VulkanRHLVertexDescription*>> matchedAttributes;
    matchedAttributes.reserve(shaderVertexAttributes.size());
    std::unordered_set<uint32_t> usedStreamSources;

    for (const auto& [semantic, shaderVertexAttribute] : shaderVertexAttributes) {
        auto vertexDescriptionIterator = std::ranges::find_if(layoutVertexDescriptions, [&semantic](const VulkanRHLVertexDescription& vertexDescription) {
            return vertexDescription.m_semantic == semantic;
        });

        if (vertexDescriptionIterator == layoutVertexDescriptions.end()
            || !createInfo.m_shaderPipeline.ValidateVertexAttribute(semantic, vertexDescriptionIterator->m_format)) {
            Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
            return nullptr;
        }

        matchedAttributes.emplace_back(&shaderVertexAttribute, &*vertexDescriptionIterator);
        usedStreamSources.insert(vertexDescriptionIterator->m_streamSource);
    }

    std::unordered_map<uint32_t, uint32_t> streamToBindingMap(usedStreamSources.size());
    std::unordered_map<uint32_t, uint32_t> bindingToStreamMap(usedStreamSources.size());
    std::vector<vk::VertexInputBindingDescription> vulkanVertexBindingDescriptions;
    vulkanVertexBindingDescriptions.reserve(usedStreamSources.size());

    uint32_t vulkanBindingIndex = 0;
    for (const VulkanRHLStreamDescription& streamDescription : createInfo.m_vertexLayout->GetStreamDescriptions()) {
        if (!usedStreamSources.contains(streamDescription.m_streamSource))
            continue;

        streamToBindingMap.emplace(streamDescription.m_streamSource, vulkanBindingIndex);
        bindingToStreamMap.emplace(vulkanBindingIndex, streamDescription.m_streamSource);
        vulkanVertexBindingDescriptions.emplace_back(vulkanBindingIndex, streamDescription.m_vertexStride, RHLInputRateToVulkan(streamDescription.m_vertexInputRate));
        ++vulkanBindingIndex;
    }

    std::vector<vk::VertexInputAttributeDescription> vulkanVertexAttributeDescriptions;
    vulkanVertexAttributeDescriptions.reserve(matchedAttributes.size());

    for (const auto& [shaderVertexAttribute, vertexDescription] : matchedAttributes) {
        auto bindingIterator = streamToBindingMap.find(vertexDescription->m_streamSource);
        if (bindingIterator == streamToBindingMap.end()) {
            Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
            return nullptr;
        }

        vulkanVertexAttributeDescriptions.emplace_back(
            shaderVertexAttribute->m_location,
            bindingIterator->second,
            Photon::Vulkan::RHLFormatToVulkan(vertexDescription->m_format),
            vertexDescription->m_streamOffset
            );
    }

    VulkanPipelineVertexInputState* instance = Photon::AllocateObject<VulkanPipelineVertexInputState>(inOutCreateParams);
    instance->m_vulkanVertexAttributeDescriptions = std::move(vulkanVertexAttributeDescriptions);
    instance->m_vulkanVertexBindingDescriptions = std::move(vulkanVertexBindingDescriptions);
    instance->m_bindingToStreamMap = std::move(bindingToStreamMap);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

const std::vector<vk::VertexInputAttributeDescription>& VulkanPipelineVertexInputState::GetVulkanVertexAttributeDescriptions() const {
    return m_vulkanVertexAttributeDescriptions;
}

const std::vector<vk::VertexInputBindingDescription>& VulkanPipelineVertexInputState::GetVulkanVertexBindingDescriptions() const {
    return m_vulkanVertexBindingDescriptions;
}

uint32_t VulkanPipelineVertexInputState::GetBindingsCount() const {
    return m_bindingToStreamMap.size();
}

uint32_t VulkanPipelineVertexInputState::GetStreamForBinding(uint32_t bindingIndex) const {
    return m_bindingToStreamMap.at(bindingIndex);
}

vk::VertexInputRate VulkanPipelineVertexInputState::RHLInputRateToVulkan(VulkanRHLVertexInputRate rhlInputRate) {
    switch (rhlInputRate) {
        case VulkanRHLVertexInputRate::PerVertex:
            return vk::VertexInputRate::eVertex;
        case VulkanRHLVertexInputRate::PerInstance:
            return vk::VertexInputRate::eInstance;
        default:
            throw std::runtime_error("");
    }
}
