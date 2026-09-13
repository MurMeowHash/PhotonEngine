#pragma once

#include <vulkan/vulkan.hpp>
#include "Configurations/VulkanAllocationPreset.h"
#include <vector>

struct VulkanAllocationTier {
    vk::DeviceSize m_pageSize;
    vk::DeviceSize m_maxAllowedAllocation;
};

struct VulkanAllocationTierComparator {
    bool operator ()(const VulkanAllocationTier& lhs, const VulkanAllocationTier& rhs) const {
        return lhs.m_maxAllowedAllocation < rhs.m_maxAllowedAllocation;
    }
};

class VulkanMemoryHeap {
public:
    VulkanMemoryHeap(vk::MemoryHeap handle, const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition);

    [[nodiscard]] uint32_t GetTiersSize() const;
    [[nodiscard]] bool TryFindAllocationTier(vk::DeviceSize allocationSize, size_t& tierIndex) const;
    [[nodiscard]] VulkanAllocationTier GetTierByIndex(size_t tierIndex) const;
private:
    vk::MemoryHeap m_handle;
    std::vector<VulkanAllocationTier> m_allocationTiers;

    void FormAllocationTiers(const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition);
    void OrderTiers();
};
