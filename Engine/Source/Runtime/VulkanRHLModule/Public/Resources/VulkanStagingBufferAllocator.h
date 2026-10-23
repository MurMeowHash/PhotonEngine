#pragma once

#include "VulkanBuffer.h"
#include "FactoryGlobals.h"
#include "CoreGlobals.h"
#include <queue>
#include <unordered_map>

class VulkanDevice;

struct VulkanStagingBufferAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
};

struct VulkanStagingBufferAllocation {
    VulkanBuffer* m_stagingBuffer = nullptr;
    vk::DeviceSize m_stagingBufferSize{};

    [[nodiscard]] bool IsValid() const;
    void Invalidate();
};

class VulkanStagingBufferAllocator {
public:
    static VulkanStagingBufferAllocator* Create(const VulkanStagingBufferAllocatorCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    VulkanStagingBufferAllocation AllocateBuffer(vk::DeviceSize bufferSize);
    void FreeBuffer(VulkanStagingBufferAllocation& bufferAllocation);
private:
    VulkanDevice* m_vulkanDevice = nullptr;
    std::unordered_map<uint64_t, std::queue<VulkanBuffer*>> m_bufferPool;

    VulkanBuffer* CreateStagingBuffer(vk::DeviceSize bufferSize) const;
};