#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

class VulkanInstance;

struct VulkanDebuggerCreateInfo {
    VulkanInstance* m_vulkanInstance;
};

class VulkanDebugger {
public:
    [[nodiscard]] bool Create(const VulkanDebuggerCreateInfo& createInfo);
private:
    vk::raii::DebugUtilsMessengerEXT m_handle = nullptr;

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                                vk::DebugUtilsMessageTypeFlagsEXT type,
                                                                const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
};