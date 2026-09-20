#pragma once
#include "VulkanComputePipeline.h"
#include "VulkanGraphicsPipeline.h"

namespace Photon::Vulkan::PipelineFactory {
    VulkanGraphicsPipeline* CreateGraphicsPipeline(const VulkanGraphicsPipelineCreateInfo& createInfo, bool* isValid = nullptr);
    VulkanComputePipeline* CreateComputePipeline(const VulkanComputePipelineCreateInfo& createInfo, bool* isValid = nullptr);
}