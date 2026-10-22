#pragma once

#include <vulkan/vulkan.hpp>
#include <MemoryPage.h>
#include "VulkanBuffer.h"

struct VulkanBufferCreateInfo;

struct VulkanBufferPageCreateInfo {
    VulkanBufferCreateInfo m_bufferCreateInfo;
};

class VulkanBufferPage final : public MemoryPage<vk::DeviceSize, VulkanBufferSlice> {
protected:
    void InitializeMemoryObject(VulkanBufferSlice& memoryObject, const MemoryInitializeInfo<vk::DeviceSize>& memInitializeInfo) override;
public:
    ~VulkanBufferPage() override;
    explicit VulkanBufferPage();
    explicit VulkanBufferPage(const VulkanBufferPageCreateInfo& createInfo);
};

enum class DedicatedMemoryRequirementsFlags : uint32_t;

struct VulkanBufferPagesAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
    //Ignored when pages are not pooled
    vk::DeviceSize pageSize;
    bool m_pooledPages;
};

class VulkanBufferPagesAllocator final : public MemoryPagesAllocator<vk::DeviceSize, VulkanBufferSlice, VulkanBufferPage> {
protected:
    VulkanBufferPage AllocatePage(vk::DeviceSize pageSize) override;
    bool MemoryBelongsToPage(const VulkanBufferPage &memoryPage, const VulkanBufferSlice &memory) override;
public:
    explicit VulkanBufferPagesAllocator(const VulkanBufferPagesAllocatorCreateInfo& createInfo);

private:
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};