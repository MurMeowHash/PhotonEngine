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

    const bool useSingleBuffers = bufferSize > m_pageSize;
    std::unordered_map<VulkanRHLBufferUsage, VulkanBufferPagesAllocator>& targetBuffers = useSingleBuffers ? m_singleBuffers : m_pooledBuffers;

    auto bufferUsageIterator = targetBuffers.find(bufferUsage);
    if (bufferUsageIterator == targetBuffers.end()) {
        VulkanBufferPagesAllocatorCreateInfo createInfo{};
        createInfo.m_vulkanDevice = m_vulkanDevice;
        createInfo.m_usageFlags = vulkanBufferUsage;
        createInfo.m_memoryPropertyFlags = m_memoryPropertyFlags;
        createInfo.m_dedicatedRequirements = DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred;
        createInfo.pageSize = m_pageSize;
        createInfo.m_pooledPages = !useSingleBuffers;
        auto bufferEmplaceIterator = targetBuffers.emplace(bufferUsage, VulkanBufferPagesAllocator(createInfo));
        bufferUsageIterator = bufferEmplaceIterator.first;
    }

    VulkanBufferSlice bufferSlice = bufferUsageIterator->second.Allocate(bufferSize, bufferAlignment);
    VulkanBufferSliceAllocation bufferSliceAllocation;
    bufferSliceAllocation.m_bufferSlice = bufferSlice;
    bufferSliceAllocation.m_useSingleBuffer = useSingleBuffers;
    bufferSliceAllocation.m_bufferUsage = bufferUsage;
    bufferSliceAllocation.m_updateFrequency = VulkanRHLBufferUpdateFrequency::None;
    return bufferSliceAllocation;
}

void GeneralVulkanBufferSubAllocator::FreeBuffer(VulkanBufferSliceAllocation &bufferSlice) {
    std::unordered_map<VulkanRHLBufferUsage, VulkanBufferPagesAllocator>& targetBuffers = bufferSlice.m_useSingleBuffer ? m_singleBuffers : m_pooledBuffers;
    auto bufferPageAllocatorIterator = targetBuffers.find(bufferSlice.m_bufferUsage);
    if (bufferPageAllocatorIterator == targetBuffers.end())
        return;

    bufferPageAllocatorIterator->second.Free(bufferSlice.m_bufferSlice);
}

GeneralVulkanBufferSubAllocator::GeneralVulkanBufferSubAllocator(VulkanDevice* vulkanDevice, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize pageSize)
: VulkanBufferSubAllocator(vulkanDevice, memoryPropertyFlags, pageSize) { }

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