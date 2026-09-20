#pragma once
#include "Shaders/VulkanShaderModule.h"

class VulkanComputePipelineDescriptor {
    friend class VulkanComputePipeline;
public:
    VulkanComputePipelineDescriptor& SetComputeShader(VulkanShaderModule* computeShaderModule) {
        assert(computeShaderModule->GetStage() == vk::ShaderStageFlagBits::eCompute);
        m_computeShaderModule = computeShaderModule;
        return *this;
    }

    [[nodiscard]] size_t GetIdentifier() const {
        throw std::runtime_error("Not implemented exception");
    }
private:
    VulkanShaderModule* m_computeShaderModule = nullptr;
};
