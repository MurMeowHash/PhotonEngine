#include "../Public/Memory/VulkanMemoryPage.h"
#include "Device/VulkanDevice.h"
#include "Memory/VulkanMemoryCommon.h"

void VulkanDeviceMemoryPage::InitializeMemoryObject(VulkanDeviceMemory &memoryObject, const MemoryInitializeInfo<vk::DeviceSize> &memInitializeInfo) {
    VulkanDeviceMemoryInitializeInfo initializeInfo{};
    initializeInfo.m_vulkanDevice = m_vulkanDevice;
    initializeInfo.m_handle = m_memory.GetHandle();
    initializeInfo.m_memoryTypeIndex = m_memory.GetMemoryTypeIndex();
    initializeInfo.m_dedicated = m_memory.IsDedicated();
    initializeInfo.m_memInfo = memInitializeInfo;
    initializeInfo.m_mappedRange = m_memory.GetMappedRange();
    memoryObject.Initialize(initializeInfo);
}

bool VulkanDeviceMemoryPage::OwnsMemory(const VulkanDeviceMemory &memory) const {
    return m_memory.GetHandle() == memory.GetHandle();
}

VulkanDeviceMemoryPage::~VulkanDeviceMemoryPage() {
    if (m_memory.IsMemoryMapped())
        m_memory.UnMapMemory();

    Photon::VulkanMemoryCommon::FreeDeviceMemory(m_vulkanDevice->GetRawHandle(), m_memory);
}

VulkanDeviceMemoryPage::VulkanDeviceMemoryPage(const VulkanDeviceMemoryPageCreateInfo &createInfo)
: m_vulkanDevice(createInfo.m_vulkanDevice) {
    m_memory = Photon::VulkanMemoryCommon::AllocateDeviceMemory(m_vulkanDevice, createInfo.m_pageSize,
        createInfo.m_memoryTypeIndex, createInfo.m_dedicatedAllocationInfo);

    if (m_memory.IsMemoryMappable())
        m_memory.MapMemory();

    InvalidateMemory();
}

std::unique_ptr<VulkanDeviceMemoryPage> VulkanDeviceMemoryPagesAllocator::AllocatePage(vk::DeviceSize pageSize) {
    VulkanDeviceMemoryPageCreateInfo createInfo{};
    createInfo.m_vulkanDevice = m_vulkanDevice;
    createInfo.m_pageSize = pageSize;
    createInfo.m_memoryTypeIndex = m_memoryTypeIndex;
    createInfo.m_dedicatedAllocationInfo = nullptr;
    return std::make_unique<VulkanDeviceMemoryPage>(createInfo);
}

VulkanDeviceMemoryPagesAllocator::VulkanDeviceMemoryPagesAllocator(const VulkanMemoryPagesAllocatorCreateInfo &createInfo)
: MemoryPagesAllocator(createInfo.m_pageSize, createInfo.m_releaseEmptyPages),
m_vulkanDevice(createInfo.m_vulkanDevice), m_memoryTypeIndex(createInfo.m_memoryTypeIndex) { }

std::unique_ptr<VulkanDeviceMemoryPage> VulkanDeviceMemoryPagesAllocatorSeparate::AllocatePage(vk::DeviceSize pageSize,
    VkMemoryDedicatedAllocateInfo * const &dedicatedAllocationInfo) {
    VulkanDeviceMemoryPageCreateInfo createInfo{};
    createInfo.m_vulkanDevice = m_vulkanDevice;
    createInfo.m_pageSize = pageSize;
    createInfo.m_memoryTypeIndex = m_memoryTypeIndex;
    createInfo.m_dedicatedAllocationInfo = dedicatedAllocationInfo;
    return std::make_unique<VulkanDeviceMemoryPage>(createInfo);
}

VulkanDeviceMemoryPagesAllocatorSeparate::VulkanDeviceMemoryPagesAllocatorSeparate(const VulkanDeviceMemoryPagesAllocatorSeparateCreateInfo &createInfo)
: MemoryPagesAllocatorSeparate(), m_vulkanDevice(createInfo.m_vulkanDevice), m_memoryTypeIndex(createInfo.m_memoryTypeIndex) { }
