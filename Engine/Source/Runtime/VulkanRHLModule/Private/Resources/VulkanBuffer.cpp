#include "../../Public/Resources/VulkanBuffer.h"
#include "Device/VulkanDevice.h"
#include "Memory/VulkanMemoryCommon.h"

bool VulkanBuffer::IsDedicated() const {
    return m_allocation.m_memory.IsDedicated();
}

void VulkanBuffer::CommitPendingTransitionScope() {
    CommitResourceTransitionScope(m_pendingTransitionScope);
}

VulkanBuffer::~VulkanBuffer() {
    m_vulkanDevice->GetMemoryProvider()->FreeMemory(m_allocation);
}

VulkanBuffer* VulkanBuffer::Create(const VulkanBufferCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::BufferCreateInfo vulkanCreateInfo{};
    vulkanCreateInfo.size = createInfo.m_size;
    vulkanCreateInfo.usage = createInfo.m_usageFlags;
    vulkanCreateInfo.sharingMode = vk::SharingMode::eExclusive;

    vk::ResultValue<vk::raii::Buffer> bufferWrapper = createInfo.m_vulkanDevice->GetHandle().createBuffer(vulkanCreateInfo);
    if (bufferWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanBuffer* instance = Photon::AllocateObject<VulkanBuffer>(inOutCreateParams);
    instance->m_handle = std::move(bufferWrapper.value);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    Photon::Result allocateResult = instance->AllocateBufferMemory(createInfo.m_memoryPropertyFlags, createInfo.m_dedicatedRequirements);
    Photon::PushResult(allocateResult, inOutCreateParams);
    return instance;
}

vk::Buffer VulkanBuffer::GetHandle() const {
    return *m_handle;
}

void VulkanBuffer::TrackLastTransitionScope(const VulkanPipelineBufferTransitionScope &transitionScope) {
    m_pendingTransitionScope = transitionScope;
}

Photon::Result VulkanBuffer::WriteData(void *data, vk::DeviceSize dataSize) {
    void* writeRange;
    if (!m_allocation.m_memory.TryGetMappedRange(writeRange))
        return Photon::Result::UnknownFailure;

    memcpy(writeRange, data, dataSize);
    return Photon::Result::Success;
}

void* VulkanBuffer::GetDataWriteDest() const {
    return m_allocation.m_memory.GetWriteRange();
}

Photon::Result VulkanBuffer::AllocateBufferMemory(vk::MemoryPropertyFlags memoryPropertyFlags, DedicatedMemoryRequirementsFlags dedicatedRequirements) {
    m_allocation = m_vulkanDevice->GetMemoryProvider()->AllocateBufferMemory(m_handle, memoryPropertyFlags, dedicatedRequirements);
    if (!m_allocation.IsValid())
        return Photon::Result::UnknownFailure;

    vk::Result memoryBindResult = Photon::VulkanMemoryCommon::BindBufferMemory(m_vulkanDevice->GetRawHandle(), m_allocation.m_memory, *m_handle);
    if (memoryBindResult != vk::Result::eSuccess)
        return Photon::Result::UnknownFailure;

    return Photon::Result::Success;
}

void VulkanBufferSlice::InvalidateMemory() {
    Initialize({});
    InitializeMemory({0, 0, 0});
}

VulkanBufferSlice::VulkanBufferSlice() {
    InvalidateMemory();
}

VulkanBufferSlice::VulkanBufferSlice(const VulkanBufferSliceInitializeInfo &initializeInfo) {
    Initialize(initializeInfo);
}

void VulkanBufferSlice::Initialize(const VulkanBufferSliceInitializeInfo &initializeInfo) {
    m_buffer = initializeInfo.m_buffer;
    InitializeMemory(initializeInfo.m_memInfo);
}

VulkanBuffer * VulkanBufferSlice::GetBuffer() const {
    return m_buffer;
}

bool VulkanBufferSlice::IsValid() const {
    return m_buffer != nullptr;
}