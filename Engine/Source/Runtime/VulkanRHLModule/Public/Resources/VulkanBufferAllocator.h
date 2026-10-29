#pragma once

#include <unordered_map>
#include "VulkanBufferPage.h"
#include "VulkanUtils.h"
#include "VulkanBufferChunk.h"

class VulkanDevice;

struct VulkanBufferSliceAllocation {
    VulkanBufferSlice m_bufferSlice;
    bool m_useSingleBuffer;
    vk::BufferUsageFlags m_bufferUsage;
    vk::MemoryPropertyFlags m_memoryProperties;

    [[nodiscard]] bool IsValid() const;
    void Invalidate();
};

struct VulkanPermanentBufferAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::DeviceSize m_pageSize;
};

class VulkanPermanentBufferAllocator {
public:
    explicit VulkanPermanentBufferAllocator(const VulkanPermanentBufferAllocatorCreateInfo& createInfo);
    static VulkanPermanentBufferAllocator* Create(const VulkanPermanentBufferAllocatorCreateInfo& createInfo, 
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    VulkanBufferSliceAllocation AllocateBuffer(vk::DeviceSize bufferSize, vk::MemoryPropertyFlags memoryProperties, vk::BufferUsageFlags bufferUsage);
    void FreeBuffer(VulkanBufferSliceAllocation& buffer);

private:
    using PooledBuffersMap = std::unordered_map<vk::BufferUsageFlags, VulkanBufferPagesAllocator, Photon::Vulkan::FlagsHasher<vk::BufferUsageFlagBits>>;

    VulkanDevice* m_vulkanDevice = nullptr;
    vk::DeviceSize m_pageSize{};
    std::unordered_map<vk::MemoryPropertyFlags, PooledBuffersMap, Photon::Vulkan::FlagsHasher<vk::MemoryPropertyFlagBits>> m_pooledBuffers;
    VulkanBufferPagesAllocatorSeparate m_singleBuffers;

    VulkanBufferPagesAllocator& GetPooledBuffersAllocator(vk::MemoryPropertyFlags memoryProperties, vk::BufferUsageFlags bufferUsage);
};

struct VulkanTemporaryBufferAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::DeviceSize m_chunkSize;
    vk::MemoryPropertyFlags m_memoryProperties;
    vk::BufferUsageFlags m_usageFlags;
};

class VulkanTemporaryBufferAllocator {
public:
    explicit VulkanTemporaryBufferAllocator(const VulkanTemporaryBufferAllocatorCreateInfo& createInfo);
    static VulkanTemporaryBufferAllocator* Create(const VulkanTemporaryBufferAllocatorCreateInfo& createInfo, 
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    
    VulkanBufferSliceAllocation AllocateBuffer(vk::DeviceSize bufferSize, vk::BufferUsageFlags bufferUsage);
    
private:
    vk::DeviceSize m_chunkSize{};
    vk::MemoryPropertyFlags m_memoryProperties{};
    
    VulkanBufferChunkAllocator m_pooledBuffers;
    VulkanBufferChunkAllocatorSeparate m_singleBuffers;
};