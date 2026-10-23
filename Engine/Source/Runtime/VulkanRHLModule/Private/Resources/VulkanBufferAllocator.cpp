#include "../../Public/Resources/VulkanBufferAllocator.h"
#include "VulkanUtils.h"
#include "Device/VulkanDevice.h"
#include "Memory/IVulkanDeviceMemoryProvider.h"

VulkanBufferSubAllocator::VulkanBufferSubAllocator(VulkanDevice* vulkanDevice, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize pageSize)
: m_vulkanDevice(vulkanDevice), m_memoryPropertyFlags(memoryPropertyFlags), m_pageSize(pageSize) { }

vk::DeviceSize VulkanBufferSubAllocator::GetBufferAlignment(vk::BufferUsageFlags bufferUsage) const {
    vk::PhysicalDeviceLimits deviceLimits = m_vulkanDevice->GetProperties().limits;
    constexpr float defaultAlignment = 16;
    vk::DeviceSize alignment = defaultAlignment;
    return alignment;
}

VulkanBufferSliceAllocation GeneralVulkanBufferSubAllocator::AllocateBuffer(vk::DeviceSize bufferSize, VulkanRHLBufferUsage bufferUsage) {
    vk::BufferUsageFlags vulkanBufferUsage = Photon::Vulkan::RHLBufferUsageToVulkan(bufferUsage);
    vk::DeviceSize bufferAlignment = GetBufferAlignment(vulkanBufferUsage);

    VulkanBufferSliceAllocation bufferSliceAllocation;
    bufferSliceAllocation.m_useSingleBuffer = bufferSize > m_pageSize;
    bufferSliceAllocation.m_bufferUsage = bufferUsage;
    bufferSliceAllocation.m_updateFrequency = VulkanRHLBufferUpdateFrequency::None;

    if (bufferSliceAllocation.m_useSingleBuffer) {
        bufferSliceAllocation.m_bufferSlice = m_singleBuffers.Allocate(bufferSize, bufferAlignment, vulkanBufferUsage);
        return bufferSliceAllocation;
    }

    auto bufferUsageIterator = m_pooledBuffers.find(bufferUsage);
    if (bufferUsageIterator == m_pooledBuffers.end()) {
        VulkanBufferPagesAllocatorCreateInfo createInfo{};
        createInfo.m_vulkanDevice = m_vulkanDevice;
        createInfo.m_usageFlags = vulkanBufferUsage;
        createInfo.m_memoryPropertyFlags = m_memoryPropertyFlags;
        createInfo.m_dedicatedRequirements = DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred;
        createInfo.m_pageSize = m_pageSize;
        createInfo.m_releaseEmptyPages = true;
        bufferUsageIterator = m_pooledBuffers.try_emplace(bufferUsage, createInfo).first;
    }

    bufferSliceAllocation.m_bufferSlice = bufferUsageIterator->second.Allocate(bufferSize, bufferAlignment);
    return bufferSliceAllocation;
}

void GeneralVulkanBufferSubAllocator::FreeBuffer(VulkanBufferSliceAllocation &bufferSlice) {
    if (bufferSlice.m_useSingleBuffer) {
        m_singleBuffers.Free(bufferSlice.m_bufferSlice);
        return;
    }

    auto bufferPageAllocatorIterator = m_pooledBuffers.find(bufferSlice.m_bufferUsage);
    if (bufferPageAllocatorIterator == m_pooledBuffers.end())
        return;

    bufferPageAllocatorIterator->second.Free(bufferSlice.m_bufferSlice);
}

GeneralVulkanBufferSubAllocator::GeneralVulkanBufferSubAllocator(VulkanDevice* vulkanDevice, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize pageSize)
: VulkanBufferSubAllocator(vulkanDevice, memoryPropertyFlags, pageSize),
m_singleBuffers(VulkanBufferPagesAllocatorSeparateCreateInfo{vulkanDevice, memoryPropertyFlags, DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred}) { }

VulkanBufferAllocator::~VulkanBufferAllocator() {
    for (std::pair<const VulkanRHLBufferUpdateFrequency, VulkanBufferSubAllocator *>& subAllocator: m_bufferSubAllocators) {
        delete subAllocator.second;
    }
}

VulkanBufferAllocator* VulkanBufferAllocator::Create(const VulkanBufferAllocatorCreateInfo &createInfo,InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanBufferAllocator* instance = Photon::AllocateObject<VulkanBufferAllocator>(inOutCreateParams);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    instance->AddBufferSubAllocator<GeneralVulkanBufferSubAllocator>(VulkanRHLBufferUpdateFrequency::Static);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanBufferSliceAllocation VulkanBufferAllocator::AllocateBuffer(vk::DeviceSize bufferSize, VulkanRHLBufferUpdateFrequency updateFrequency, VulkanRHLBufferUsage bufferUsage) {
    assert(updateFrequency != VulkanRHLBufferUpdateFrequency::None);
    VulkanBufferSliceAllocation bufferSliceAllocation = m_bufferSubAllocators[updateFrequency]->AllocateBuffer(bufferSize, bufferUsage);
    bufferSliceAllocation.m_updateFrequency = updateFrequency;
    return bufferSliceAllocation;
}

void VulkanBufferAllocator::FreeBuffer(VulkanBufferSliceAllocation &buffer) {
    assert(buffer.m_updateFrequency != VulkanRHLBufferUpdateFrequency::None);
    m_bufferSubAllocators[buffer.m_updateFrequency]->FreeBuffer(buffer);
}