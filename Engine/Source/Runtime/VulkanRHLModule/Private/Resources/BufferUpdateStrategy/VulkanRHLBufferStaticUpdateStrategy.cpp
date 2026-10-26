#include "../../../Public/Resources/BufferUpdateStrategy/VulkanRHLBufferStaticUpdateStrategy.h"
#include "VulkanRHLCommandWorkContext.h"
#include "Resources/VulkanStagingBufferAllocator.h"
#include "Device/VulkanDevice.h"

void* VulkanRHLBufferStaticUpdateStrategy::BeginWrite(const VulkanRHLBufferUpdateContext &updateContext, uint64_t writeOffset, uint64_t writeSize) {
    if (!updateContext.m_bufferAllocation->IsValid()) {
        *updateContext.m_bufferAllocation = updateContext.m_allocateFunc();
        if (updateContext.m_bufferAllocation->IsValid())
            return nullptr;
    }

    VulkanStagingBufferAllocation stagingBufferAllocation = updateContext.m_vulkanDevice->GetStagingBufferAllocator()->AllocateBuffer(writeSize);
    if (!stagingBufferAllocation.IsValid())
        return nullptr;

    m_pendingWrite = VulkanRHLStaticBufferPendingWrite(writeOffset, writeSize, stagingBufferAllocation);
    return stagingBufferAllocation.m_stagingBuffer->GetDataWriteDest();
}

void VulkanRHLBufferStaticUpdateStrategy::EndWrite(const VulkanRHLBufferUpdateContext &updateContext) {
    if (!m_pendingWrite.IsValid())
        return;

    VulkanStagingBufferAllocation stagingBufferAllocation = m_pendingWrite.m_stagingBufferAllocation;
    VulkanBufferSlice stagingBufferSlice({stagingBufferAllocation.m_stagingBuffer,
        {m_pendingWrite.m_writeSize,0,0}});
    VulkanBufferSlice activeAllocationSubSlice({updateContext.m_bufferAllocation->m_bufferSlice.GetBuffer(),
        {m_pendingWrite.m_writeSize, updateContext.m_bufferAllocation->m_bufferSlice.GetOffset() + m_pendingWrite.m_writeOffset,
            updateContext.m_bufferAllocation->m_bufferSlice.GetAlignmentOffset()}});
    updateContext.m_workContext->GetCommandBuffer()->CopyBuffer(stagingBufferSlice, activeAllocationSubSlice);
    VulkanPipelineBarrier pipelineBarrier{};
    pipelineBarrier.TransitionBuffer(updateContext.m_bufferAllocation->m_bufferSlice.GetBuffer(),
        {vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite, vk::QueueFamilyIgnored},
        {vk::PipelineStageFlagBits2::eAllCommands, vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite, vk::QueueFamilyIgnored},
        updateContext.m_bufferAllocation->m_bufferSlice.GetOffset(), updateContext.m_bufferAllocation->m_bufferSlice.GetSize());
    updateContext.m_workContext->GetCommandBuffer()->ExecutePipelineBarrier(std::move(pipelineBarrier));
    VulkanDevice* device = updateContext.m_vulkanDevice;
    updateContext.m_workContext->AddSyncEvent(VulkanWorkSyncEvent(StaticDelegate<>([device, &stagingBufferAllocation]() {
        device->GetStagingBufferAllocator()->FreeBuffer(stagingBufferAllocation);
    })));

    m_pendingWrite.Invalidate();
}