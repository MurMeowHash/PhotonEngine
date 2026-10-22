#include "../../Public/Memory/VulkanDeviceMemory.h"

VulkanDeviceMemory::VulkanDeviceMemory() {
    InvalidateMemory();
}

VulkanDeviceMemory::VulkanDeviceMemory(const VulkanDeviceMemoryInitializeInfo &initializeInfo) {
    Initialize(initializeInfo);
}

void VulkanDeviceMemory::Initialize(const VulkanDeviceMemoryInitializeInfo &initializeInfo) {
    m_handle = initializeInfo.m_handle;
    m_memoryTypeIndex = initializeInfo.m_memoryTypeIndex;
    m_dedicated = initializeInfo.m_dedicated;
    InitializeMemory(initializeInfo.m_memInfo);
}

uint32_t VulkanDeviceMemory::GetMemoryTypeIndex() const {
    return m_memoryTypeIndex;
}

vk::DeviceMemory VulkanDeviceMemory::GetHandle() const {
    return m_handle;
}

bool VulkanDeviceMemory::IsValid() const {
    return m_handle != nullptr;
}

bool VulkanDeviceMemory::IsDedicated() const {
    return m_dedicated;
}

RuntimeAttributes& VulkanDeviceMemory::GetMemoryAttributes() {
    return m_memoryAttributes;
}

void* VulkanDeviceMemory::GetMappedRange() const {
    return m_mappedRange;
}

bool VulkanDeviceMemory::TryGetMappedRange(void*& mappedRange) const {
    if (m_mappedRange == nullptr)
        return false;

    mappedRange = m_mappedRange;
    return true;
}

Photon::Result VulkanDeviceMemory::MapMemory() {
    void* mappedRange = nullptr;
    VkResult mapResult = vkMapMemory(m_device, m_handle, m_offset, m_size, {}, &mappedRange);
    if (mapResult != VK_SUCCESS)
        return Photon::Result::UnknownFailure;

    m_mappedRange = mappedRange;
    return Photon::Result::Success;
}

void VulkanDeviceMemory::UnMapMemory() {
    if (m_mappedRange == nullptr)
        return;

    vkUnmapMemory(m_device, m_handle);
    m_mappedRange = nullptr;
}

void VulkanDeviceMemory::InvalidateMemory() {
    Initialize({});
    m_memoryAttributes.Invalidate();
}
