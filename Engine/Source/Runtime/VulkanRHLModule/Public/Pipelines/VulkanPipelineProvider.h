#pragma once
#include "Blueprints/BlueprintProvider.h"
#include "VulkanRHLPipelineDescriptor.h"
#include "VulkanGraphicsPipeline.h"

class VulkanDevice;

struct VulkanGraphicsPipelineProviderCreateInfo {
    VulkanDevice* m_vulkanDevice;
};

class VulkanGraphicsPipelineProvider final : public BlueprintProvider<VulkanGraphicsPipeline, VulkanRHLGraphicsPipelineDescriptor> {
public:
    static VulkanGraphicsPipelineProvider* Create(const VulkanGraphicsPipelineProviderCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
protected:
    VulkanGraphicsPipeline* CreateBlueprint(const VulkanRHLGraphicsPipelineDescriptor &desc, void *allocatedMemory) override;

private:
    VulkanDevice* m_vulkanDevice = nullptr;
};