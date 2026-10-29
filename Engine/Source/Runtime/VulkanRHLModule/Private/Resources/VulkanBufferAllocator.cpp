#include "../../Public/Resources/VulkanBufferAllocator.h"
#include "Device/VulkanDevice.h"
#include "Memory/IVulkanDeviceMemoryProvider.h"

bool VulkanBufferSliceAllocation::IsValid() const {
    return m_bufferSlice.IsValid();
}

void VulkanBufferSliceAllocation::Invalidate() {
    m_bufferSlice.InvalidateMemory();
}

VulkanPermanentBufferAllocator::VulkanPermanentBufferAllocator(const VulkanPermanentBufferAllocatorCreateInfo &createInfo)
: m_vulkanDevice(createInfo.m_vulkanDevice), m_pageSize(createInfo.m_pageSize),
m_singleBuffers(VulkanBufferPagesAllocatorSeparateCreateInfo{createInfo.m_vulkanDevice, DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred}) { }

VulkanPermanentBufferAllocator* VulkanPermanentBufferAllocator::Create(const VulkanPermanentBufferAllocatorCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanPermanentBufferAllocator* instance = Photon::AllocateObject<VulkanPermanentBufferAllocator>(inOutCreateParams, createInfo);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanBufferSliceAllocation VulkanPermanentBufferAllocator::AllocateBuffer(vk::DeviceSize bufferSize,
    vk::MemoryPropertyFlags memoryProperties, vk::BufferUsageFlags bufferUsage) {
    vk::DeviceSize bufferAlignment = Photon::Vulkan::GetBufferAlignment(bufferUsage);

    VulkanBufferSliceAllocation bufferSliceAllocation;
    bufferSliceAllocation.m_useSingleBuffer = bufferSize > m_pageSize;
    bufferSliceAllocation.m_bufferUsage = bufferUsage;
    bufferSliceAllocation.m_memoryProperties = memoryProperties;

    if (bufferSliceAllocation.m_useSingleBuffer) {
        bufferSliceAllocation.m_bufferSlice = m_singleBuffers.Allocate(bufferSize, bufferAlignment, bufferUsage, memoryProperties);
        return bufferSliceAllocation;
    }

    bufferSliceAllocation.m_bufferSlice = GetPooledBuffersAllocator(memoryProperties, bufferUsage).Allocate(bufferSize, bufferAlignment);
    return bufferSliceAllocation;
}

void VulkanPermanentBufferAllocator::FreeBuffer(VulkanBufferSliceAllocation &buffer) {
    if (buffer.m_useSingleBuffer) {
        m_singleBuffers.Free(buffer.m_bufferSlice);
        return;
    }

    auto pooledBuffersIterator = m_pooledBuffers.find(buffer.m_memoryProperties);
    if (pooledBuffersIterator == m_pooledBuffers.end())
        return;

    auto pagesAllocatorIterator = pooledBuffersIterator->second.find(buffer.m_bufferUsage);
    if (pagesAllocatorIterator == pooledBuffersIterator->second.end())
        return;

    pagesAllocatorIterator->second.Free(buffer.m_bufferSlice);
}

VulkanBufferPagesAllocator& VulkanPermanentBufferAllocator::GetPooledBuffersAllocator(vk::MemoryPropertyFlags memoryProperties, vk::BufferUsageFlags bufferUsage) {
    PooledBuffersMap& pooledBuffers = m_pooledBuffers[memoryProperties];
    auto pagesAllocatorIterator = pooledBuffers.find(bufferUsage);
    if (pagesAllocatorIterator != pooledBuffers.end())
        return pagesAllocatorIterator->second;

    VulkanBufferPagesAllocatorCreateInfo createInfo{};
    createInfo.m_vulkanDevice = m_vulkanDevice;
    createInfo.m_usageFlags = bufferUsage;
    createInfo.m_memoryPropertyFlags = memoryProperties;
    createInfo.m_dedicatedRequirements = DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred;
    createInfo.m_pageSize = m_pageSize;
    createInfo.m_releaseEmptyPages = true;
    return pooledBuffers.try_emplace(bufferUsage, createInfo).first->second;
}

VulkanTemporaryBufferAllocator::VulkanTemporaryBufferAllocator(const VulkanTemporaryBufferAllocatorCreateInfo &createInfo)
: m_chunkSize(createInfo.m_chunkSize), m_memoryProperties(createInfo.m_memoryProperties),
m_pooledBuffers({createInfo.m_vulkanDevice, createInfo.m_usageFlags, createInfo.m_memoryProperties,
    DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred, createInfo.m_chunkSize}),
m_singleBuffers({createInfo.m_vulkanDevice, createInfo.m_usageFlags, createInfo.m_memoryProperties,
    DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred}) { }

VulkanTemporaryBufferAllocator* VulkanTemporaryBufferAllocator::Create(const VulkanTemporaryBufferAllocatorCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanTemporaryBufferAllocator* instance = Photon::AllocateObject<VulkanTemporaryBufferAllocator>(inOutCreateParams, createInfo);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanBufferSliceAllocation VulkanTemporaryBufferAllocator::AllocateBuffer(vk::DeviceSize bufferSize, vk::BufferUsageFlags bufferUsage) {
    vk::DeviceSize bufferAlignment = Photon::Vulkan::GetBufferAlignment(bufferUsage);

    VulkanBufferSliceAllocation bufferSliceAllocation;
    bufferSliceAllocation.m_useSingleBuffer = bufferSize > m_chunkSize;
    bufferSliceAllocation.m_bufferUsage = bufferUsage;
    bufferSliceAllocation.m_memoryProperties = m_memoryProperties;

    bufferSliceAllocation.m_bufferSlice = bufferSliceAllocation.m_useSingleBuffer
        ? m_singleBuffers.Allocate(bufferSize, bufferAlignment)
        : m_pooledBuffers.Allocate(bufferSize, bufferAlignment);

    return bufferSliceAllocation;
}