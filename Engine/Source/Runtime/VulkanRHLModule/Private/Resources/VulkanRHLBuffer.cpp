#include "../../Public/Resources/VulkanRHLBuffer.h"
#include "VulkanRHLCommandWorkContext.h"
#include "Device/VulkanDevice.h"
#include "Resources/BufferUpdateStrategy/VulkanRHLBufferStaticUpdateStrategy.h"
#include "ObjectFunc.h"
#include "Resources/BufferUpdateStrategy/VulkanRHLBufferVolatileUpdateStrategy.h"

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
    instance->m_cachedVulkanMemoryProperties = Photon::Vulkan::ResolveVulkanMemoryFlagsFromUpdateFrequency(createInfo.m_rhlInfo.m_updateFrequency);
    instance->m_cachedVulkanBufferUsage =
        Photon::Vulkan::GetBufferRequiredUsageFlags(createInfo.m_rhlInfo.m_updateFrequency)
        | Photon::Vulkan::RHLBufferUsageToVulkan(createInfo.m_rhlInfo.m_usage);
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
    if (!m_activeBufferAllocation.IsValid() || m_updateFrequency == VulkanRHLBufferUpdateFrequency::Volatile)
        return;

    VulkanBufferSliceAllocation outdatedAllocation = m_activeBufferAllocation;
    VulkanDevice* capturedDevice = m_vulkanDevice;
    m_vulkanDevice->GetDeferredDeletionQueue()->Enqueue(
        VulkanDDQEntry(StaticDelegate([capturedDevice, outdatedAllocation]() mutable {
            capturedDevice->GetPermanentBufferAllocator()->FreeBuffer(outdatedAllocation);
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
        case VulkanRHLBufferUpdateFrequency::Volatile:
            m_bufferUpdateStrategy = new VulkanRHLBufferVolatileUpdateStrategy();
            break;
        default:
            throw std::runtime_error("");
    }
}

VulkanBufferSliceAllocation VulkanRHLBuffer::AllocateBufferSlice() {
    switch (m_updateFrequency) {
        case VulkanRHLBufferUpdateFrequency::Static:
            return m_vulkanDevice->GetPermanentBufferAllocator()->AllocateBuffer(m_size, m_cachedVulkanMemoryProperties, m_cachedVulkanBufferUsage);
        case VulkanRHLBufferUpdateFrequency::Volatile:
            return m_vulkanDevice->GetTemporaryBufferAllocator()->AllocateBuffer(m_size, m_cachedVulkanBufferUsage);
        default:
            throw std::runtime_error("");
    }
}