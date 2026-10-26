#pragma once
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

class VulkanPipeline {
public:
    virtual ~VulkanPipeline() = default;
    [[nodiscard]] vk::Pipeline GetHandle() const;
protected:
    vk::raii::Pipeline m_handle = nullptr;
    vk::raii::PipelineLayout m_pipelineLayout = nullptr;
};
