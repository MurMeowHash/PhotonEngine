#include "../../Public/Memory/VulkanDeviceMemory.h"
#include "Device/VulkanDevice.h"

VulkanDeviceMemory::VulkanDeviceMemory() {
    InvalidateMemory();
}

VulkanDeviceMemory::VulkanDeviceMemory(const VulkanDeviceMemoryInitializeInfo &initializeInfo) {
    Initialize(initializeInfo);
}

void VulkanDeviceMemory::Initialize(const VulkanDeviceMemoryInitializeInfo &initializeInfo) {
    m_vulkanDevice = initializeInfo.m_vulkanDevice;
    m_handle = initializeInfo.m_handle;
    m_memoryTypeIndex = initializeInfo.m_memoryTypeIndex;
    m_dedicated = initializeInfo.m_dedicated;
    m_allocationTier = initializeInfo.m_allocationTier;
    m_mappedRange = initializeInfo.m_mappedRange;
    InitializeMemory(initializeInfo.m_memInfo);
}

uint32_t VulkanDeviceMemory::GetMemoryTypeIndex() const {
    return m_memoryTypeIndex;
}

uint32_t VulkanDeviceMemory::GetAllocationTier() const {
    return m_allocationTier;
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

bool VulkanDeviceMemory::IsMemoryMappable() const {
    vk::PhysicalDeviceMemoryProperties memoryProperties = m_vulkanDevice->GetMemoryProperties();
    if (m_memoryTypeIndex >= memoryProperties.memoryTypeCount)
        return false;

    return (memoryProperties.memoryTypes[m_memoryTypeIndex].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible;
}

void* VulkanDeviceMemory::GetMappedRange() const {
    return m_mappedRange;
}

void* VulkanDeviceMemory::GetWriteRange() const {
    if (m_mappedRange == nullptr)
        return nullptr;

    return static_cast<char*>(m_mappedRange) + m_offset;
}

bool VulkanDeviceMemory::IsMemoryMapped() const {
    return m_mappedRange != nullptr;
}

bool VulkanDeviceMemory::TryGetMappedRange(void*& mappedRange) const {
    if (m_mappedRange == nullptr)
        return false;

    mappedRange = m_mappedRange;
    return true;
}

Photon::Result VulkanDeviceMemory::MapMemory() {
    void* mappedRange = nullptr;
    VkResult mapResult = vkMapMemory(m_vulkanDevice->GetRawHandle(), m_handle, m_offset, m_size, {}, &mappedRange);
    if (mapResult != VK_SUCCESS)
        return Photon::Result::UnknownFailure;

    m_mappedRange = mappedRange;
    return Photon::Result::Success;
}

void VulkanDeviceMemory::UnMapMemory() {

    vkUnmapMemory(m_vulkanDevice->GetRawHandle(), m_handle);
    m_mappedRange = nullptr;
}

void VulkanDeviceMemory::InvalidateMemory() {
    Initialize({});
}
