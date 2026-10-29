#pragma once

#include "MemoryChunk.h"
#include "VulkanBuffer.h"

struct VulkanBufferChunkCreateInfo {
    VulkanBufferCreateInfo m_bufferCreateInfo;
};

class VulkanBufferChunkAllocator;

class VulkanBufferChunk final : public MemoryChunk<vk::DeviceSize, VulkanBufferSlice> {
protected:
    void InitializeMemoryObject(VulkanBufferSlice &memoryObject, const MemoryInitializeInfo<vk::DeviceSize> &memInitializeInfo) override;

public:
    ~VulkanBufferChunk() override;
    explicit VulkanBufferChunk(const VulkanBufferChunkCreateInfo& createInfo);
};

struct VulkanBufferChunkAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
    vk::DeviceSize m_chunkSize;
};

class VulkanBufferChunkAllocator final : public MemoryChunkExtractedAllocator<vk::DeviceSize, VulkanBufferSlice, VulkanBufferChunk> {
protected:
    void OnChunkExtracted(VulkanBufferChunk *chunk) override;
    VulkanBufferChunk* AllocateMemoryChunk(vk::DeviceSize chunkSize) override;

public:
    explicit VulkanBufferChunkAllocator(const VulkanBufferChunkAllocatorCreateInfo& createInfo);

private:
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};

struct VulkanBufferChunkAllocatorSeparateCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};

class VulkanBufferChunkAllocatorSeparate final : public MemoryChunkExtractedAllocatorSeparate<vk::DeviceSize, VulkanBufferSlice, VulkanBufferChunk> {
protected:
    void OnChunkExtracted(VulkanBufferChunk *chunk) override;
    VulkanBufferChunk* AllocateMemoryChunk(vk::DeviceSize chunkSize) override;

public:
    explicit VulkanBufferChunkAllocatorSeparate(const VulkanBufferChunkAllocatorSeparateCreateInfo& createInfo);

private:
    VulkanDevice* m_vulkanDevice;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};