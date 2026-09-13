#include "../../Public/Memory/VulkanDeviceAllocatorBundle.h"
#include "IAllocator.h"
#include "Memory/DefaultVulkanDedicatedDeviceAllocator.h"
#include "Memory/DefaultVulkanDeviceAllocator.h"

VulkanDeviceAllocatorBundle::VulkanDeviceAllocatorBundle(vk::raii::Device &device, uint32_t memoryTypeIndex,
    const VulkanMemoryHeap& vulkanMemoryHeap, IAllocator *allocator)
: m_device(device), m_memoryTypeIndex(memoryTypeIndex), m_vulkanMemoryHeap(vulkanMemoryHeap),
m_allocator(allocator) { }

IVulkanDeviceAllocator* VulkanDeviceAllocatorBundle::GetDeviceAllocator() {
    if (m_deviceAllocator == nullptr) {
        void* memory = m_allocator->AllocateMemory(sizeof(DefaultVulkanDeviceAllocator));
        m_deviceAllocator = new (memory) DefaultVulkanDeviceAllocator(m_device, m_vulkanMemoryHeap, m_memoryTypeIndex);
    }

    return m_deviceAllocator;
}

IVulkanDedicatedDeviceAllocator * VulkanDeviceAllocatorBundle::GetDedicatedDeviceAllocator() {
    if (m_dedicatedDeviceAllocator == nullptr) {
        void* memory = m_allocator->AllocateMemory(sizeof(DefaultVulkanDedicatedDeviceAllocator));
        m_dedicatedDeviceAllocator = new (memory) DefaultVulkanDedicatedDeviceAllocator(m_device, m_memoryTypeIndex);
    }

    return m_dedicatedDeviceAllocator;
}

bool VulkanDeviceAllocatorBundle::TryGetDeviceAllocator(IVulkanDeviceAllocator *&deviceAllocator) const {
    if (m_deviceAllocator == nullptr)
        return false;

    deviceAllocator = m_deviceAllocator;
    return true;
}

bool VulkanDeviceAllocatorBundle::TryGetDedicatedDeviceAllocator(IVulkanDedicatedDeviceAllocator *&dedicatedDeviceAllocator) const {
    if (m_dedicatedDeviceAllocator == nullptr)
        return false;

    dedicatedDeviceAllocator = m_dedicatedDeviceAllocator;
    return true;
}