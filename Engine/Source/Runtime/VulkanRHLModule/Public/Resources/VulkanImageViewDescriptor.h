#pragma once
#include "Blueprints/BlueprintDescriptor.h"
#include "vulkan/vulkan.hpp"

class VulkanImageViewDescriptor : public BlueprintDescriptor {
public:
    vk::ImageViewType m_viewType;
    vk::Format m_format;
    vk::ComponentMapping m_components;
    vk::ImageSubresourceRange m_subresourceRange;

    [[nodiscard]] size_t GetIdentifier() const override {
        size_t seed = 0;

        auto hashCombine = [&seed](size_t value) {
            seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };

        hashCombine(std::hash<VkImageViewType>{}(
            static_cast<VkImageViewType>(m_viewType)));

        hashCombine(std::hash<VkFormat>{}(
            static_cast<VkFormat>(m_format)));

        hashCombine(std::hash<VkComponentSwizzle>{}(
            static_cast<VkComponentSwizzle>(m_components.r)));
        hashCombine(std::hash<VkComponentSwizzle>{}(
            static_cast<VkComponentSwizzle>(m_components.g)));
        hashCombine(std::hash<VkComponentSwizzle>{}(
            static_cast<VkComponentSwizzle>(m_components.b)));
        hashCombine(std::hash<VkComponentSwizzle>{}(
            static_cast<VkComponentSwizzle>(m_components.a)));

        hashCombine(std::hash<VkImageAspectFlags>{}(
            static_cast<VkImageAspectFlags>(m_subresourceRange.aspectMask)));
        hashCombine(std::hash<uint32_t>{}(
            m_subresourceRange.baseMipLevel));
        hashCombine(std::hash<uint32_t>{}(
            m_subresourceRange.levelCount));
        hashCombine(std::hash<uint32_t>{}(
            m_subresourceRange.baseArrayLayer));
        hashCombine(std::hash<uint32_t>{}(
            m_subresourceRange.layerCount));

        return seed;
    }
};
