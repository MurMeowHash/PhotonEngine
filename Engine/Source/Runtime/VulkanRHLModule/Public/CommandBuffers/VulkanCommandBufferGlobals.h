#pragma once
#include "CoreUtils.h"
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

enum class VulkanCommandBufferType {
    None = 0,
    Primary = 1,
    Secondary = 2,
};

enum class VulkanCommandBufferLifetime {
    None = 0,
    ShortLived = 1,
    LongLived = 2,
};

namespace Photon::Vulkan {
    [[nodiscard]] inline vk::CommandBufferLevel DeriveLevelFromType(VulkanCommandBufferType commandBufferType) {
        switch (commandBufferType) {
            case VulkanCommandBufferType::Primary:
                return vk::CommandBufferLevel::ePrimary;
            case VulkanCommandBufferType::Secondary:
                return vk::CommandBufferLevel::eSecondary;
            default:
                throw std::runtime_error(StringFormatter("There is no supported command buffer level for type ", static_cast<int>(commandBufferType)));
        }
    }
}