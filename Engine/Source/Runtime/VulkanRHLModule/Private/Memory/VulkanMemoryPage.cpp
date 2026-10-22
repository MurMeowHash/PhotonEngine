#include "../Public/Memory/VulkanMemoryPage.h"
#include <algorithm>
#include "Memory/VulkanMemoryCommon.h"

void VulkanMemoryPage::InitializeMemoryObject(VulkanDeviceMemory &memoryObject, const MemoryInitializeInfo<vk::DeviceSize>& memInitializeInfo) {
    VulkanDeviceMemoryInitializeInfo initializeInfo{};
    initializeInfo.m_handle = m_memory.GetHandle();
    initializeInfo.m_memoryTypeIndex = memoryObject.GetMemoryTypeIndex();
    initializeInfo.m_dedicated = memoryObject.IsDedicated();
    initializeInfo.m_memInfo = memInitializeInfo;
    memoryObject.Initialize(initializeInfo);
}

VulkanMemoryPage::VulkanMemoryPage(VulkanDeviceMemory&& memory)
: MemoryPage(std::move(memory)) { }

VulkanMemoryPage VulkanMemoryPagesAllocator::AllocatePage(vk::DeviceSize pageSize) {
    Photon::VulkanMemoryCommon::AllocateDeviceMemory()
}