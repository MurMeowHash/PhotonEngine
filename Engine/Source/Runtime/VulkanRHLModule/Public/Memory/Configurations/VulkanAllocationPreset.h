#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

struct VulkanAllocationTierDefinition {
    vk::DeviceSize m_maxPageSize;
    vk::DeviceSize m_heapDivider;
    vk::DeviceSize m_maxAllocationDivider;
};

struct VulkanAllocationPresetDefinition {
    std::vector<VulkanAllocationTierDefinition> m_allocationTierDefinitions;
};

struct DesktopVulkanAllocationPresetDefinition : public VulkanAllocationPresetDefinition {
    DesktopVulkanAllocationPresetDefinition() {
        m_allocationTierDefinitions = {
            VulkanAllocationTierDefinition(16 * 1024 * 1024, 16 * 4 * 4, 3),
            VulkanAllocationTierDefinition(64 * 1024 * 1024, 16 * 4, 3),
            VulkanAllocationTierDefinition(256 * 1024 * 1024, 16, 3)
        };
    }
};