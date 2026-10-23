#pragma once

#include "VulkanDeviceMemory.h"
#include "MemoryPage.h"

class VulkanDevice;

struct VulkanDeviceMemoryPageCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::DeviceSize m_pageSize;
    uint32_t m_memoryTypeIndex;
    VkMemoryDedicatedAllocateInfo* m_dedicatedAllocationInfo;
};

class VulkanDeviceMemoryPage final : public MemoryPage<vk::DeviceSize, VulkanDeviceMemory> {
protected:
    void InitializeMemoryObject(VulkanDeviceMemory &memoryObject, const MemoryInitializeInfo<vk::DeviceSize>& memInitializeInfo) override;
public:
    [[nodiscard]] bool OwnsMemory(const VulkanDeviceMemory& memory) const override;
public:
    explicit VulkanDeviceMemoryPage(const VulkanDeviceMemoryPageCreateInfo& createInfo);
private:
    VulkanDevice* m_vulkanDevice;
};

struct VulkanMemoryPagesAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
    vk::DeviceSize m_pageSize;
    bool m_releaseEmptyPages;
};

class VulkanDeviceMemoryPagesAllocator : public MemoryPagesAllocator<vk::DeviceSize, VulkanDeviceMemory, VulkanDeviceMemoryPage> {
protected:
    [[nodiscard]] std::unique_ptr<VulkanDeviceMemoryPage> AllocatePage(vk::DeviceSize pageSize) override;
public:
    explicit VulkanDeviceMemoryPagesAllocator(const VulkanMemoryPagesAllocatorCreateInfo& createInfo);

private:
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
};

struct VulkanDeviceMemoryPagesAllocatorSeparateCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
};

class VulkanDeviceMemoryPagesAllocatorSeparate : public MemoryPagesAllocatorSeparate<vk::DeviceSize, VulkanDeviceMemory, VulkanDeviceMemoryPage, VkMemoryDedicatedAllocateInfo*> {
protected:
    [[nodiscard]] std::unique_ptr<VulkanDeviceMemoryPage> AllocatePage(vk::DeviceSize pageSize,
        VkMemoryDedicatedAllocateInfo* const &dedicatedAllocationInfo) override;

public:
    explicit VulkanDeviceMemoryPagesAllocatorSeparate(const VulkanDeviceMemoryPagesAllocatorSeparateCreateInfo& createInfo);

private:
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
};