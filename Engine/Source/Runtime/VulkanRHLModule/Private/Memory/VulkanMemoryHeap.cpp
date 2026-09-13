#include "../../Public/Memory/VulkanMemoryHeap.h"

#include <ranges>

VulkanMemoryHeap::VulkanMemoryHeap(vk::MemoryHeap handle, const VulkanAllocationPresetDefinition &vulkanAllocationPresetDefinition)
: m_handle(handle) {
    FormAllocationTiers(vulkanAllocationPresetDefinition);
}

uint32_t VulkanMemoryHeap::GetTiersSize() const {
    return m_allocationTiers.size();
}

bool VulkanMemoryHeap::TryFindAllocationTier(vk::DeviceSize allocationSize, size_t &tierIndex) const {
    for (size_t i = 0; i < m_allocationTiers.size(); i++) {
        if (allocationSize <= m_allocationTiers[i].m_maxAllowedAllocation) {
            tierIndex = i;
            return true;
        }
    }

    return false;
}

VulkanAllocationTier VulkanMemoryHeap::GetTierByIndex(size_t tierIndex) const {
    if (tierIndex >= m_allocationTiers.size())
        return VulkanAllocationTier();

    return m_allocationTiers[tierIndex];
}

void VulkanMemoryHeap::FormAllocationTiers(const VulkanAllocationPresetDefinition &vulkanAllocationPresetDefinition) {
    m_allocationTiers.clear();

    for (const VulkanAllocationTierDefinition& allocationTierDefinition : vulkanAllocationPresetDefinition.m_allocationTierDefinitions) {
        VulkanAllocationTier allocationTier;
        allocationTier.m_pageSize = std::min(m_handle.size / allocationTierDefinition.m_heapDivider, allocationTierDefinition.m_maxPageSize);
        allocationTier.m_maxAllowedAllocation = allocationTier.m_pageSize / allocationTierDefinition.m_maxAllocationDivider;
        m_allocationTiers.emplace_back(allocationTier);
    }

    OrderTiers();
}

void VulkanMemoryHeap::OrderTiers() {
    std::ranges::sort(m_allocationTiers, VulkanAllocationTierComparator());
}
