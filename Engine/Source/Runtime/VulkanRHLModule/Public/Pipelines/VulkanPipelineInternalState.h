#pragma once

#include "FactoryGlobals.h"
#include "CoreGlobals.h"
#include "Shaders/VulkanShaderPipeline.h"
#include "VertexFactory/VulkanRHLVertexLayout.h"
#include <unordered_map>

struct VulkanPipelineVertexInputStateCreateInfo {
    const VulkanShaderPipeline& m_shaderPipeline;
    VulkanRHLVertexLayout* m_vertexLayout;
};

class VulkanPipelineVertexInputState {
public:
    static VulkanPipelineVertexInputState* Create(const VulkanPipelineVertexInputStateCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    [[nodiscard]] const std::vector<vk::VertexInputAttributeDescription>& GetVulkanVertexAttributeDescriptions() const;
    [[nodiscard]] const std::vector<vk::VertexInputBindingDescription>& GetVulkanVertexBindingDescriptions() const;

    [[nodiscard]] uint32_t GetBindingsCount() const;
    [[nodiscard]] uint32_t GetStreamForBinding(uint32_t bindingIndex) const;

private:
    std::vector<vk::VertexInputAttributeDescription> m_vulkanVertexAttributeDescriptions;
    std::vector<vk::VertexInputBindingDescription> m_vulkanVertexBindingDescriptions;

    std::unordered_map<uint32_t, uint32_t> m_bindingToStreamMap;

    static vk::VertexInputRate RHLInputRateToVulkan(VulkanRHLVertexInputRate rhlInputRate);
};