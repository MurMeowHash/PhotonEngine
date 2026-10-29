#include "../../Public/Resources/VulkanBufferChunk.h"
#include "Device/VulkanDevice.h"

void VulkanBufferChunk::InitializeMemoryObject(VulkanBufferSlice &memoryObject, const MemoryInitializeInfo<vk::DeviceSize> &memInitializeInfo) {
    VulkanBufferSliceInitializeInfo initializeInfo{};
    initializeInfo.m_buffer = m_memory.GetBuffer();
    initializeInfo.m_memInfo = memInitializeInfo;
    memoryObject.Initialize(initializeInfo);
}

VulkanBufferChunk::~VulkanBufferChunk() {
    delete m_memory.GetBuffer();
}

VulkanBufferChunk::VulkanBufferChunk(const VulkanBufferChunkCreateInfo &createInfo) {
    InOutCreateParams<Photon::Result> inOut{};
    VulkanBuffer* buffer = VulkanBuffer::Create(createInfo.m_bufferCreateInfo, &inOut);
    if (inOut.m_result != Photon::Result::Success) {
        delete buffer;
        return;
    }

    VulkanBufferSliceInitializeInfo sliceInitInfo{buffer, {createInfo.m_bufferCreateInfo.m_size, 0, 0}};
    m_memory.Initialize(sliceInitInfo);
    InvalidateMemory();
}

void VulkanBufferChunkAllocator::OnChunkExtracted(VulkanBufferChunk *chunk) {
    m_vulkanDevice->GetDeferredDeletionQueue()->Enqueue(
        VulkanDDQEntry{StaticDelegate<>{[this, chunk]() {
            chunk->InvalidateMemory();
            AddFreeChunk(chunk);
        }}});
}

VulkanBufferChunk* VulkanBufferChunkAllocator::AllocateMemoryChunk(vk::DeviceSize chunkSize) {
    VulkanBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.m_vulkanDevice = m_vulkanDevice;
    bufferCreateInfo.m_size = chunkSize;
    bufferCreateInfo.m_usageFlags = m_usageFlags;
    bufferCreateInfo.m_memoryPropertyFlags = m_memoryPropertyFlags;
    bufferCreateInfo.m_dedicatedRequirements = m_dedicatedRequirements;
    return new VulkanBufferChunk({bufferCreateInfo});
}

VulkanBufferChunkAllocator::VulkanBufferChunkAllocator(const VulkanBufferChunkAllocatorCreateInfo &createInfo)
: MemoryChunkExtractedAllocator(createInfo.m_chunkSize), m_vulkanDevice(createInfo.m_vulkanDevice),
m_usageFlags(createInfo.m_usageFlags), m_memoryPropertyFlags(createInfo.m_memoryPropertyFlags),
m_dedicatedRequirements(createInfo.m_dedicatedRequirements) { }

void VulkanBufferChunkAllocatorSeparate::OnChunkExtracted(VulkanBufferChunk *chunk) {
    m_vulkanDevice->GetDeferredDeletionQueue()->Enqueue(
        VulkanDDQEntry{StaticDelegate{[chunk]() {
            delete chunk;
        }}});
}

VulkanBufferChunk* VulkanBufferChunkAllocatorSeparate::AllocateMemoryChunk(vk::DeviceSize chunkSize) {
    VulkanBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.m_vulkanDevice = m_vulkanDevice;
    bufferCreateInfo.m_size = chunkSize;
    bufferCreateInfo.m_usageFlags = m_usageFlags;
    bufferCreateInfo.m_memoryPropertyFlags = m_memoryPropertyFlags;
    bufferCreateInfo.m_dedicatedRequirements = m_dedicatedRequirements;
    return new VulkanBufferChunk({bufferCreateInfo});
}

VulkanBufferChunkAllocatorSeparate::VulkanBufferChunkAllocatorSeparate(const VulkanBufferChunkAllocatorSeparateCreateInfo &createInfo)
: m_vulkanDevice(createInfo.m_vulkanDevice), m_usageFlags(createInfo.m_usageFlags),
m_memoryPropertyFlags(createInfo.m_memoryPropertyFlags), m_dedicatedRequirements(createInfo.m_dedicatedRequirements) { }
