#include "../../Public/Resources/VulkanRHLBuffer.h"
#include "VulkanRHLCommandWorkContext.h"
#include "Device/VulkanDevice.h"
#include "Resources/BufferUpdateStrategy/VulkanRHLBufferStaticUpdateStrategy.h"
#include "ObjectFunc.h"

VulkanRHLBuffer::~VulkanRHLBuffer() {
    Clear();
    delete m_bufferUpdateStrategy;
}

VulkanRHLBuffer* VulkanRHLBuffer::Create(const VulkanRHLBufferCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLBuffer* instance = Photon::AllocateObject<VulkanRHLBuffer>(inOutCreateParams);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    instance->m_size = createInfo.m_rhlInfo.m_size;
    instance->m_elementSize = createInfo.m_rhlInfo.m_elementSize;
    instance->m_usage = createInfo.m_rhlInfo.m_usage;
    instance->m_updateFrequency = createInfo.m_rhlInfo.m_updateFrequency;
    instance->CreateUpdateStrategy();
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

void* VulkanRHLBuffer::BeginWrite(VulkanRHLCommandWorkContext *workContext, uint64_t writeOffset, uint64_t writeSize) {
    VulkanRHLBufferUpdateContext updateContext{
        m_vulkanDevice,
        &m_activeBufferAllocation,
        workContext,
        m_bufferSliceAllocationFunc
    };

    return m_bufferUpdateStrategy->BeginWrite(updateContext, writeOffset, writeSize);
}

void VulkanRHLBuffer::EndWrite(VulkanRHLCommandWorkContext *workContext) {
    VulkanRHLBufferUpdateContext updateContext{
        m_vulkanDevice,
        &m_activeBufferAllocation,
        workContext,
        m_bufferSliceAllocationFunc
    };

    m_bufferUpdateStrategy->EndWrite(updateContext);
}

void VulkanRHLBuffer::Clear() {
    VulkanBufferSliceAllocation outdatedAllocation = m_activeBufferAllocation;
    VulkanDevice* capturedDevice = m_vulkanDevice;
    m_vulkanDevice->GetDeferredDeletionQueue()->Enqueue(
        VulkanDDQEntry(StaticDelegate<>([capturedDevice, outdatedAllocation]() mutable {
            capturedDevice->GetBufferAllocator()->FreeBuffer(outdatedAllocation);
    })));

    m_activeBufferAllocation.Invalidate();
}

uint64_t VulkanRHLBuffer::GetSize() const {
    return m_size;
}

VulkanBufferSlice VulkanRHLBuffer::GetBackingHandle() const {
    return m_activeBufferAllocation.m_bufferSlice;
}

uint64_t VulkanRHLBuffer::GetElementSize() const {
    return m_elementSize;
}

void VulkanRHLBuffer::CreateUpdateStrategy() {
    switch (m_updateFrequency) {
        case VulkanRHLBufferUpdateFrequency::Static:
            m_bufferUpdateStrategy = new VulkanRHLBufferStaticUpdateStrategy();
            break;
        default:
            throw std::runtime_error("");
    }
}

VulkanBufferSliceAllocation VulkanRHLBuffer::AllocateBufferSlice() {
    return m_vulkanDevice->GetBufferAllocator()->AllocateBuffer(m_size, m_updateFrequency, m_usage);
}