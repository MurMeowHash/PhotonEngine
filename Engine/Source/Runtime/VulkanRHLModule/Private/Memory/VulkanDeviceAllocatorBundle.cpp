#include "../../Public/Memory/VulkanDeviceAllocatorBundle.h"
#include "IAllocator.h"
#include "Memory/DefaultVulkanSeparateDeviceAllocator.h"
#include "Memory/DefaultVulkanDeviceAllocator.h"

VulkanDeviceAllocatorBundle::VulkanDeviceAllocatorBundle(const VulkanDeviceAllocatorBundleCreateInfo &createInfo)
: m_vulkanDevice(createInfo.m_vulkanDevice), m_memoryTypeIndex(createInfo.m_memoryTypeIndex),
m_vulkanMemoryHeap(createInfo.m_vulkanMemoryHeap), m_allocator(createInfo.m_allocator),
m_releaseEmptyPages(createInfo.m_releaseEmptyPages) { }

IVulkanDeviceAllocator* VulkanDeviceAllocatorBundle::GetDeviceAllocator() {
    if (m_deviceAllocator == nullptr) {
        void* memory = m_allocator->AllocateMemory(sizeof(DefaultVulkanDeviceAllocator));
        DefaultVulkanDeviceAllocatorCreateInfo createInfo{m_vulkanDevice, m_vulkanMemoryHeap, m_memoryTypeIndex, m_releaseEmptyPages};
        m_deviceAllocator = new (memory) DefaultVulkanDeviceAllocator(createInfo);
    }

    return m_deviceAllocator;
}

IVulkanSeparateDeviceAllocator* VulkanDeviceAllocatorBundle::GetSeparateDeviceAllocator() {
    if (m_separateDeviceAllocator == nullptr) {
        void* memory = m_allocator->AllocateMemory(sizeof(DefaultVulkanSeparateDeviceAllocator));
        DefaultVulkanSeparateDeviceAllocatorCreateInfo createInfo{m_vulkanDevice, m_memoryTypeIndex};
        m_separateDeviceAllocator = new (memory) DefaultVulkanSeparateDeviceAllocator(createInfo);
    }

    return m_separateDeviceAllocator;
}

bool VulkanDeviceAllocatorBundle::TryGetDeviceAllocator(IVulkanDeviceAllocator *&deviceAllocator) const {
    if (m_deviceAllocator == nullptr)
        return false;

    deviceAllocator = m_deviceAllocator;
    return true;
}

bool VulkanDeviceAllocatorBundle::TryGetSeparateDeviceAllocator(IVulkanSeparateDeviceAllocator *&separateDeviceAllocator) const {
    if (m_separateDeviceAllocator == nullptr)
        return false;

    separateDeviceAllocator = m_separateDeviceAllocator;
    return true;
}