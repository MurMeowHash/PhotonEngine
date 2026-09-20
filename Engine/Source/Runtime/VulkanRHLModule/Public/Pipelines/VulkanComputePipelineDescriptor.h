#pragma once
#include "VulkanPipelineDescriptor.h"
#include "Shaders/VulkanShaderModule.h"

class VulkanComputePipelineDescriptor : public VulkanPipelineDescriptor {
    friend class VulkanComputePipeline;
public:
    VulkanComputePipelineDescriptor& SetComputeShader(VulkanShaderModule* computeShaderModule) {
        assert(computeShaderModule->GetStage() == vk::ShaderStageFlagBits::eCompute);
        m_computeShaderModule = computeShaderModule;
        return *this;
    }

    [[nodiscard]] size_t GetIdentifier() const {

    }
private:
    VulkanShaderModule* m_computeShaderModule = nullptr;
};
