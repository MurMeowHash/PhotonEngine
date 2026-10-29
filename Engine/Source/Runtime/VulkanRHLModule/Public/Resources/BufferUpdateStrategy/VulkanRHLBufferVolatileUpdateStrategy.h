#pragma once
#include "IVulkanRHLBufferUpdateStrategy.h"

class VulkanRHLBufferVolatileUpdateStrategy : public IVulkanRHLBufferUpdateStrategy {
public:
    void* BeginWrite(const VulkanRHLBufferUpdateContext &updateContext, uint64_t writeOffset, uint64_t writeSize) override;
    void EndWrite(const VulkanRHLBufferUpdateContext &updateContext) override;
};
