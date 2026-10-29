#pragma once

#include "VulkanRHLResourceDefinitions.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "ObjectFunc.h"
#include "VulkanBuffer.h"
#include "VulkanBufferAllocator.h"
#include "BufferUpdateStrategy/IVulkanRHLBufferUpdateStrategy.h"

class VulkanDevice;
class VulkanRHLCommandWorkContext;

struct RHLBufferCreateInfo {
    uint64_t m_size;
    uint64_t m_elementSize;
    VulkanRHLBufferUpdateFrequency m_updateFrequency;
    VulkanRHLBufferUsage m_usage;
};

struct VulkanRHLBufferCreateInfo {
    VulkanDevice* m_vulkanDevice;
    RHLBufferCreateInfo m_rhlInfo;
};

class VulkanRHLBuffer {
public:
    ~VulkanRHLBuffer();
    static VulkanRHLBuffer* Create(const VulkanRHLBufferCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    void* BeginWrite(VulkanRHLCommandWorkContext* workContext, uint64_t writeOffset, uint64_t writeSize);
    void EndWrite(VulkanRHLCommandWorkContext* workContext);
    void Clear();
    [[nodiscard]] uint64_t GetSize() const;
    [[nodiscard]] VulkanBufferSlice GetBackingHandle() const;
    [[nodiscard]] uint64_t GetElementSize() const;

private:
    VulkanDevice* m_vulkanDevice = nullptr;
    uint64_t m_size{};
    uint64_t m_elementSize{};
    VulkanRHLBufferUsage m_usage{};
    VulkanRHLBufferUpdateFrequency m_updateFrequency{};
    VulkanBufferSliceAllocation m_activeBufferAllocation{};

    IVulkanRHLBufferUpdateStrategy* m_bufferUpdateStrategy = nullptr;

    vk::MemoryPropertyFlags m_cachedVulkanMemoryProperties{};
    vk::BufferUsageFlags m_cachedVulkanBufferUsage{};

    ObjectFunc<VulkanRHLBuffer, VulkanBufferSliceAllocation> m_bufferSliceAllocationFunc{this, &VulkanRHLBuffer::AllocateBufferSlice};

    void CreateUpdateStrategy();
    VulkanBufferSliceAllocation AllocateBufferSlice();
};