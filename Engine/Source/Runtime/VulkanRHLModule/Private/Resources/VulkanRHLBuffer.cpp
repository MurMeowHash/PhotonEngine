#include "../../Public/Resources/VulkanRHLBuffer.h"
#include "VulkanRHLCommandWorkContext.h"
#include "Device/VulkanDevice.h"

VulkanRHLBuffer::~VulkanRHLBuffer() {
    Clear();
}

void VulkanRHLBuffer::Clear() {
    VulkanBufferSliceAllocation outdatedAllocation = m_activeBufferAllocation;
    m_vulkanDevice->GetDeferredDeletionQueue()->Enqueue(VulkanDDQEntry(StaticDelegate<>([this, &outdatedAllocation]() {
        m_vulkanDevice->GetBufferAllocator()->FreeBuffer(outdatedAllocation);
    })));

    m_activeBufferAllocation.Invalidate();
}

uint64_t VulkanRHLBuffer::GetSize() const {
    return m_size;
}

void VulkanRHLBuffer::Initialize(VulkanDevice *vulkanDevice, uint64_t size, VulkanRHLBufferUsage usage) {
    m_vulkanDevice = vulkanDevice;
    m_size = size;
    m_usage = usage;
}

void* VulkanRHLStaticBuffer::BeginWrite(VulkanRHLCommandWorkContext* workContext, uint64_t writeOffset, uint64_t writeSize) {
    if (!m_activeBufferAllocation.IsValid()) {
        m_activeBufferAllocation = m_vulkanDevice->GetBufferAllocator()->AllocateBuffer(m_size, VulkanRHLBufferUpdateFrequency::Static, m_usage);
        if (m_activeBufferAllocation.IsValid())
            return nullptr;
    }

    VulkanStagingBufferAllocation stagingBufferAllocation = m_vulkanDevice->GetStagingBufferAllocator()->AllocateBuffer(writeSize);
    if (!stagingBufferAllocation.IsValid())
        return nullptr;

    m_pendingWrite = VulkanRHLStaticBufferPendingWrite(writeOffset, writeSize, stagingBufferAllocation);
    return stagingBufferAllocation.m_stagingBuffer->GetDataWriteDest();
}

void VulkanRHLStaticBuffer::EndWrite(VulkanRHLCommandWorkContext* workContext) {
    if (!m_pendingWrite.IsValid())
        return;

    VulkanStagingBufferAllocation stagingBufferAllocation = m_pendingWrite.m_stagingBufferAllocation;
    VulkanBufferSlice stagingBufferSlice({stagingBufferAllocation.m_stagingBuffer,
        {m_pendingWrite.m_writeSize,0,0}});
    VulkanBufferSlice activeAllocationSubSlice({m_activeBufferAllocation.m_bufferSlice.GetBuffer(),
        {m_pendingWrite.m_writeSize, m_activeBufferAllocation.m_bufferSlice.GetOffset() + m_pendingWrite.m_writeOffset,
            m_activeBufferAllocation.m_bufferSlice.GetAlignmentOffset()}});
    workContext->GetCommandBuffer()->CopyBuffer(stagingBufferSlice, activeAllocationSubSlice);
    VulkanPipelineBarrier pipelineBarrier{};
    pipelineBarrier.TransitionBuffer(m_activeBufferAllocation.m_bufferSlice.GetBuffer(),
        {vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite, vk::QueueFamilyIgnored},
        {vk::PipelineStageFlagBits2::eAllCommands, vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite, vk::QueueFamilyIgnored},
        m_activeBufferAllocation.m_bufferSlice.GetOffset(), m_activeBufferAllocation.m_bufferSlice.GetSize());
    workContext->GetCommandBuffer()->ExecutePipelineBarrier(std::move(pipelineBarrier));
    workContext->AddSyncEvent(VulkanWorkSyncEvent(StaticDelegate<>([this, &stagingBufferAllocation]() {
        m_vulkanDevice->GetStagingBufferAllocator()->FreeBuffer(stagingBufferAllocation);
    })));

    m_pendingWrite.Invalidate();
}

VulkanRHLStaticBuffer* VulkanRHLStaticBuffer::Create(const VulkanRHLStaticBufferCreateInfo &createInfo,
                                                     InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLStaticBuffer* instance = Photon::AllocateObject<VulkanRHLStaticBuffer>(inOutCreateParams);
    instance->Initialize(createInfo.m_vulkanDevice, createInfo.m_size, createInfo.m_usage);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}
