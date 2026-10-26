#pragma once

#include "IVulkanRHLBufferUpdateStrategy.h"
#include "Resources/VulkanStagingBufferAllocator.h"

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

class VulkanRHLBufferStaticUpdateStrategy : public IVulkanRHLBufferUpdateStrategy {
public:
    void* BeginWrite(const VulkanRHLBufferUpdateContext &updateContext, uint64_t writeOffset, uint64_t writeSize) override;
    void EndWrite(const VulkanRHLBufferUpdateContext& updateContext) override;

private:
    VulkanRHLStaticBufferPendingWrite m_pendingWrite{};
};