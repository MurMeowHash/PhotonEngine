#include "../../../Public/Resources/BufferUpdateStrategy/VulkanRHLBufferVolatileUpdateStrategy.h"
#include "Resources/VulkanBufferAllocator.h"

void* VulkanRHLBufferVolatileUpdateStrategy::BeginWrite(const VulkanRHLBufferUpdateContext &updateContext, uint64_t writeOffset, uint64_t writeSize) {
    *updateContext.m_bufferAllocation = updateContext.m_allocateFunc();
    if (!updateContext.m_bufferAllocation->IsValid())
        return nullptr;

    return static_cast<char*>(updateContext.m_bufferAllocation->m_bufferSlice.GetSliceWriteDest()) + writeOffset;
}

void VulkanRHLBufferVolatileUpdateStrategy::EndWrite(const VulkanRHLBufferUpdateContext &updateContext) {

}