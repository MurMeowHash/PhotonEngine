#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

class VulkanSemaphore {
public:
    virtual ~VulkanSemaphore() = default;
    [[nodiscard]] const vk::raii::Semaphore& GetHandle() const;
protected:
    vk::raii::Semaphore m_handle = nullptr;
};