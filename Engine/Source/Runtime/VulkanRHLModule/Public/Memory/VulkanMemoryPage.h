#pragma once

#include "VulkanDeviceMemory.h"
#include "MemoryPage.h"

class VulkanDevice;

class VulkanDeviceMemoryPage final : public MemoryPage<vk::DeviceSize, VulkanDeviceMemory> {
protected:
    void InitializeMemoryObject(VulkanDeviceMemory &memoryObject, const MemoryInitializeInfo<vk::DeviceSize>& memInitializeInfo) override;
public:
    explicit VulkanMemoryPage(VulkanDeviceMemory&& memory); // should be create info with device
};

struct VulkanMemoryPagesAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
};

class VulkanDeviceMemoryPagesAllocator : public MemoryPagesAllocator<vk::DeviceSize, VulkanDeviceMemory, VulkanDeviceMemoryPage> {
protected:
    VulkanDeviceMemoryPage AllocatePage(vk::DeviceSize pageSize) override;
    bool MemoryBelongsToPage(const VulkanDeviceMemoryPage &memoryPage, const VulkanDeviceMemory &memory) override;

public:
    explicit VulkanDeviceMemoryPagesAllocator(const VulkanMemoryPagesAllocatorCreateInfo& createInfo);
private:
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
};
