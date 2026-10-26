#pragma once

#include "IFunc.h"
#include <cstdint>

class VulkanRHLCommandWorkContext;
class VulkanDevice;
class VulkanBufferSliceAllocation;

struct VulkanRHLBufferUpdateContext {
    VulkanDevice* m_vulkanDevice;
    VulkanBufferSliceAllocation* m_bufferAllocation;
    VulkanRHLCommandWorkContext* m_workContext;
    const IFunc<VulkanBufferSliceAllocation>& m_allocateFunc;
};

class IVulkanRHLBufferUpdateStrategy {
public:
    virtual ~IVulkanRHLBufferUpdateStrategy() = default;
    virtual void* BeginWrite(const VulkanRHLBufferUpdateContext& updateContext, uint64_t writeOffset, uint64_t writeSize) = 0;
    virtual void EndWrite(const VulkanRHLBufferUpdateContext& updateContext) = 0;
};