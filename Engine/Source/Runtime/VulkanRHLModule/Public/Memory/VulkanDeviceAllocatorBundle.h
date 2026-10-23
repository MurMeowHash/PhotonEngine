#pragma once

#include "IVulkanSeparateDeviceAllocator.h"
#include "IVulkanDeviceAllocator.h"
#include "VulkanMemoryHeap.h"

class IAllocator;
class VulkanDevice;

struct VulkanDeviceAllocatorBundleCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
    const VulkanMemoryHeap& m_vulkanMemoryHeap;
    IAllocator* m_allocator;
    bool m_releaseEmptyPages;
};

class VulkanDeviceAllocatorBundle {
public:
    explicit VulkanDeviceAllocatorBundle(const VulkanDeviceAllocatorBundleCreateInfo& createInfo);
    IVulkanDeviceAllocator* GetDeviceAllocator();
    IVulkanSeparateDeviceAllocator* GetSeparateDeviceAllocator();
    [[nodiscard]] bool TryGetDeviceAllocator(IVulkanDeviceAllocator*& deviceAllocator) const;
    [[nodiscard]] bool TryGetSeparateDeviceAllocator(IVulkanSeparateDeviceAllocator*& separateDeviceAllocator) const;

private:
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
    const VulkanMemoryHeap& m_vulkanMemoryHeap;
    IAllocator* m_allocator;
    bool m_releaseEmptyPages;

    IVulkanDeviceAllocator* m_deviceAllocator = nullptr;
    IVulkanSeparateDeviceAllocator* m_separateDeviceAllocator = nullptr;
};
