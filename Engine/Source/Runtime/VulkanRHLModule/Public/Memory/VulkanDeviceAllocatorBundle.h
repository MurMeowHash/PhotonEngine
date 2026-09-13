#pragma once

#include "IVulkanDedicatedDeviceAllocator.h"
#include "IVulkanDeviceAllocator.h"
#include "VulkanMemoryHeap.h"

class IAllocator;

class VulkanDeviceAllocatorBundle {
public:
    VulkanDeviceAllocatorBundle(vk::raii::Device& device, uint32_t memoryTypeIndex,
        const VulkanMemoryHeap& vulkanMemoryHeap, IAllocator* allocator);
    IVulkanDeviceAllocator* GetDeviceAllocator();
    IVulkanDedicatedDeviceAllocator* GetDedicatedDeviceAllocator();
    [[nodiscard]] bool TryGetDeviceAllocator(IVulkanDeviceAllocator*& deviceAllocator) const;
    [[nodiscard]] bool TryGetDedicatedDeviceAllocator(IVulkanDedicatedDeviceAllocator*& dedicatedDeviceAllocator) const;

private:
    vk::raii::Device& m_device;
    uint32_t m_memoryTypeIndex;
    const VulkanMemoryHeap& m_vulkanMemoryHeap;
    IAllocator* m_allocator;

    IVulkanDeviceAllocator* m_deviceAllocator = nullptr;
    IVulkanDedicatedDeviceAllocator* m_dedicatedDeviceAllocator = nullptr;
};
