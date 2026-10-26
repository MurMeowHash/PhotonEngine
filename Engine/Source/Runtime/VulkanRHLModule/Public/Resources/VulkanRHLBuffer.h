#pragma once

#include "VulkanRHLResourceDefinitions.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanBuffer.h"
#include "VulkanBufferAllocator.h"
#include "VulkanStagingBufferAllocator.h"

class VulkanDevice;
class VulkanRHLCommandWorkContext;

struct VulkanRHLBufferCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint64_t m_size;
    VulkanRHLBufferUpdateFrequency m_updateFrequency;
    VulkanRHLBufferUsage m_usage;
};

struct VulkanRHLStaticBufferPendingWrite {
    uint64_t m_writeOffset;
    uint64_t m_writeSize;
    VulkanStagingBufferAllocation m_stagingBufferAllocation;

    [[nodiscard]] bool IsValid() const {
        return m_stagingBufferAllocation.IsValid();
    }

    void Invalidate() {
        m_stagingBufferAllocation.Invalidate();
    }
};

class VulkanRHLBuffer {
public:
    virtual ~VulkanRHLBuffer();
    virtual void* BeginWrite(VulkanRHLCommandWorkContext* workContext, uint64_t writeOffset, uint64_t writeSize) = 0;
    virtual void EndWrite(VulkanRHLCommandWorkContext* workContext) = 0;
    void Clear();
    [[nodiscard]] uint64_t GetSize() const;
    [[nodiscard]] VulkanBufferSlice GetBackingHandle() const;
    [[nodiscard]] uint64_t GetElementSize() const;

protected:
    VulkanDevice* m_vulkanDevice = nullptr;
    uint64_t m_size{};
    uint64_t m_elementSize{};
    VulkanRHLBufferUsage m_usage{};
    VulkanBufferSliceAllocation m_activeBufferAllocation = VulkanBufferSliceAllocation();

    void Initialize(VulkanDevice* vulkanDevice, uint64_t size, uint64_t elementSize, VulkanRHLBufferUsage usage);
};

struct VulkanRHLStaticBufferCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint64_t m_size;
    uint64_t m_elementSize;
    VulkanRHLBufferUsage m_usage;
};

class VulkanRHLStaticBuffer final : public VulkanRHLBuffer {
public:
    void* BeginWrite(VulkanRHLCommandWorkContext* workContext, uint64_t writeOffset, uint64_t writeSize) override;
    void EndWrite(VulkanRHLCommandWorkContext* workContext) override;
public:
    static VulkanRHLStaticBuffer* Create(const VulkanRHLStaticBufferCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

private:
    VulkanRHLStaticBufferPendingWrite m_pendingWrite{};
};