#include "../Public/Memory/VulkanMemoryPage.h"
#include <algorithm>
#include "MemoryRange.h"

struct VulkanMemoryRangeComparator {
    bool operator()(const VulkanMemoryRange& lhs, const VulkanMemoryRange& rhs) const {
        return lhs.GetBegin() < rhs.GetBegin();
    }
};

VulkanMemoryPage::VulkanMemoryPage(VulkanDeviceMemory&& memory)
: m_memory(std::move(memory)) {
    InvalidateMemory();
}

bool VulkanMemoryPage::TryAllocateFromRange(vk::DeviceSize requiredMemorySize, vk::DeviceSize alignment, VulkanDeviceMemory &allocatedMemory) {
    auto freeRange = std::ranges::find_if(m_freeRanges, [requiredMemorySize, alignment](const VulkanMemoryRange& memoryRange) {
        return requiredMemorySize <= memoryRange.GetAlignedSize(alignment);
    });

    if (freeRange == m_freeRanges.end())
        return false;

    VulkanDeviceMemoryInitializer memoryInitializer;
    memoryInitializer.m_handle = m_memory.GetHandle();
    memoryInitializer.m_offset = freeRange->GetAlignedBegin(alignment);
    memoryInitializer.m_alignmentOffset = freeRange->GetAlignedBegin(alignment) - freeRange->GetBegin();
    memoryInitializer.m_size = requiredMemorySize;
    memoryInitializer.m_memoryTypeIndex = m_memory.GetMemoryTypeIndex();
    memoryInitializer.m_dedicated = m_memory.IsDedicated();

    allocatedMemory.InitializeMemory(memoryInitializer);

    if (requiredMemorySize >= freeRange->GetAlignedSize(alignment))
        m_freeRanges.erase(freeRange);
    else
        freeRange->ShrinkForwardAligned(requiredMemorySize, alignment);

    return true;
}

void VulkanMemoryPage::ReturnMemory(VulkanDeviceMemory &memory) {
    vk::DeviceSize occupationOffset = memory.GetMemoryOccupationOffset();
    VulkanMemoryRange returnedRange = VulkanMemoryRange(occupationOffset, occupationOffset + memory.GetAllocationSize());
    auto returnedRangePosInterator = std::ranges::lower_bound(m_freeRanges, returnedRange, VulkanMemoryRangeComparator());
    auto returnedRangeIterator = m_freeRanges.insert(returnedRangePosInterator, returnedRange);
    MergeRangesForward(returnedRangeIterator, 1);
    MergeRangesBackward(returnedRangeIterator, 1);

    std::erase_if(m_freeRanges, [](const VulkanMemoryRange& memoryRange) {
        return memoryRange.IsCoalesced();
    });

    memory.InvalidateMemory();
}

VulkanDeviceMemory& VulkanMemoryPage::GetPageMemory() {
    return m_memory;
}

void VulkanMemoryPage::InvalidateMemory() {
    m_freeRanges.clear();
    m_freeRanges.emplace_back(0, m_memory.GetAllocationSize());
}

void VulkanMemoryPage::MergeRangesForward(std::vector<VulkanMemoryRange>::iterator currentRange, VulkanRangeIteratorOffset neighborOffset) {
    auto neighborRange = currentRange + neighborOffset;

    if (neighborRange == m_freeRanges.end())
        return;

    if (currentRange->GetEnd() == neighborRange->GetBegin()) {
        currentRange->OverrideEnd(neighborRange->GetEnd());
        neighborRange->MarkCoalesced();
        MergeRangesForward(currentRange, neighborOffset + 1);
    }
}

void VulkanMemoryPage::MergeRangesBackward(std::vector<VulkanMemoryRange>::iterator currentRange, VulkanRangeIteratorOffset neighborOffset) {
    auto neighborRange = currentRange - neighborOffset;

    if (neighborRange < m_freeRanges.begin())
        return;

    if (currentRange->GetBegin() == neighborRange->GetEnd()) {
        currentRange->OverrideBegin(neighborRange->GetBegin());
        neighborRange->MarkCoalesced();
        MergeRangesBackward(currentRange, neighborOffset + 1);
    }
}
