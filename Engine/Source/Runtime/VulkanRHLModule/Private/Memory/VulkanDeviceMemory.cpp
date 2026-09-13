#include "../../Public/Memory/VulkanDeviceMemory.h"

VulkanDeviceMemory::VulkanDeviceMemory() {
    InvalidateMemory();
}

VulkanDeviceMemory::VulkanDeviceMemory(const VulkanDeviceMemoryInitializer& memoryInitializer) {
    InitializeMemory(memoryInitializer);
}

void VulkanDeviceMemory::InitializeMemory(const VulkanDeviceMemoryInitializer& memoryInitializer) {
    m_handle = memoryInitializer.m_handle;
    m_offset = memoryInitializer.m_offset;
    m_alignmentOffset = memoryInitializer.m_alignmentOffset;
    m_size = memoryInitializer.m_size;
    m_memoryTypeIndex = memoryInitializer.m_memoryTypeIndex;
    m_dedicated = memoryInitializer.m_dedicated;
}

uint32_t VulkanDeviceMemory::GetSize() const {
    return m_size;
}

uint32_t VulkanDeviceMemory::GetAllocationSize() const {
    return m_size + m_alignmentOffset;
}

uint32_t VulkanDeviceMemory::GetMemoryTypeIndex() const {
    return m_memoryTypeIndex;
}

vk::DeviceMemory VulkanDeviceMemory::GetHandle() const {
    return m_handle;
}

uint32_t VulkanDeviceMemory::GetOffset() const {
    return m_offset;
}

vk::DeviceSize VulkanDeviceMemory::GetAlignmentOffset() const {
    return m_alignmentOffset;
}

vk::DeviceSize VulkanDeviceMemory::GetMemoryOccupationOffset() const {
    return m_offset - m_alignmentOffset;
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

void VulkanDeviceMemory::InvalidateMemory() {
    VulkanDeviceMemoryInitializer initializer{nullptr, 0, 0, 0, 0, false};
    InitializeMemory(initializer);
    m_memoryAttributes.Invalidate();
}
