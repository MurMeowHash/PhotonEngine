#pragma once

#include <vulkan/vulkan.hpp>
#include <MemoryPage.h>
#include "VulkanBuffer.h"

struct VulkanBufferPageCreateInfo {
    VulkanBufferCreateInfo m_bufferCreateInfo;
};

class VulkanBufferPage final : public MemoryPage<vk::DeviceSize, VulkanBufferSlice> {
protected:
    void InitializeMemoryObject(VulkanBufferSlice& memoryObject, const MemoryInitializeInfo<vk::DeviceSize>& memInitializeInfo) override;
public:
    [[nodiscard]] bool OwnsMemory(const VulkanBufferSlice& memory) const override;

    ~VulkanBufferPage() override;
    explicit VulkanBufferPage(const VulkanBufferPageCreateInfo& createInfo);
};

enum class DedicatedMemoryRequirementsFlags : uint32_t;

struct VulkanBufferPagesAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
    vk::DeviceSize m_pageSize;
    bool m_releaseEmptyPages;
};

class VulkanBufferPagesAllocator final : public MemoryPagesAllocator<vk::DeviceSize, VulkanBufferSlice, VulkanBufferPage> {
protected:
    [[nodiscard]] std::unique_ptr<VulkanBufferPage> AllocatePage(vk::DeviceSize pageSize) override;
public:
    explicit VulkanBufferPagesAllocator(const VulkanBufferPagesAllocatorCreateInfo& createInfo);

private:
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};

struct VulkanBufferPagesAllocatorSeparateCreateInfo {
    VulkanDevice* m_vulkanDevice;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};

class VulkanBufferPagesAllocatorSeparate final : public MemoryPagesAllocatorSeparate<vk::DeviceSize, VulkanBufferSlice, VulkanBufferPage,
    vk::BufferUsageFlags, vk::MemoryPropertyFlags> {
protected:
    [[nodiscard]] std::unique_ptr<VulkanBufferPage> AllocatePage(vk::DeviceSize pageSize, const vk::BufferUsageFlags& usageFlags,
        const vk::MemoryPropertyFlags& memoryPropertyFlags) override;
public:
    explicit VulkanBufferPagesAllocatorSeparate(const VulkanBufferPagesAllocatorSeparateCreateInfo& createInfo);

private:
    VulkanDevice* m_vulkanDevice;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};
