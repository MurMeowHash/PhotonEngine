#pragma once
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

class VulkanPipeline {
public:
    [[nodiscard]] const vk::raii::Pipeline& GetHandle() const;
protected:
    vk::raii::Pipeline m_handle = nullptr;
};
