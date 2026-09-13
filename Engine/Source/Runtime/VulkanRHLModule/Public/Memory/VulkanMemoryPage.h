#pragma once

#include "VulkanDeviceMemory.h"
#include <vector>
#include "MemoryRange.h"

using VulkanMemoryRange = MemoryRange<vk::DeviceSize>;
using VulkanRangeIteratorOffset = std::iter_difference_t<std::vector<VulkanMemoryRange>::iterator>;

class VulkanMemoryPage {
public:
    explicit VulkanMemoryPage(VulkanDeviceMemory&& memory);

    [[nodiscard]] bool TryAllocateFromRange(vk::DeviceSize requiredMemorySize, vk::DeviceSize alignment, VulkanDeviceMemory& allocatedMemory);
    void ReturnMemory(VulkanDeviceMemory& memory);
    [[nodiscard]] VulkanDeviceMemory& GetPageMemory();
    void InvalidateMemory();

private:
    VulkanDeviceMemory m_memory;
    std::vector<VulkanMemoryRange> m_freeRanges;

    void MergeRangesForward(std::vector<VulkanMemoryRange>::iterator currentRange, VulkanRangeIteratorOffset neighborOffset);
    void MergeRangesBackward(std::vector<VulkanMemoryRange>::iterator currentRange, VulkanRangeIteratorOffset neighborOffset);
};
