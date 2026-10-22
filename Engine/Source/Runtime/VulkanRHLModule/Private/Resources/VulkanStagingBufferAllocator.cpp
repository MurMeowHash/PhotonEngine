#include "../../Public/Resources/VulkanStagingBufferAllocator.h"
#include <bit>
#include "Memory/IVulkanDeviceMemoryProvider.h"

bool VulkanStagingBufferAllocation::IsValid() const {
    return m_stagingBuffer != nullptr;
}

void VulkanStagingBufferAllocation::Invalidate() {
    m_stagingBuffer = nullptr;
    m_stagingBufferSize = 0ull;
}

VulkanStagingBufferAllocator* VulkanStagingBufferAllocator::Create(const VulkanStagingBufferAllocatorCreateInfo &createInfo,
                                                                   InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanStagingBufferAllocator* instance = Photon::AllocateObject<VulkanStagingBufferAllocator>(inOutCreateParams);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanStagingBufferAllocation VulkanStagingBufferAllocator::AllocateBuffer(vk::DeviceSize bufferSize) {
    vk::DeviceSize roundedSize = std::bit_ceil(bufferSize);
    auto bufferPoolIterator = m_bufferPool.find(roundedSize);
    if (bufferPoolIterator == m_bufferPool.end()) {
        auto emplaceIterator = m_bufferPool.emplace(roundedSize, std::queue<VulkanBuffer*>{});
        bufferPoolIterator = emplaceIterator.first;
    }

    if (!bufferPoolIterator->second.empty()) {
        VulkanBuffer* buffer = bufferPoolIterator->second.front();
        return VulkanStagingBufferAllocation(buffer, roundedSize);
    }

    VulkanBuffer* stagingBuffer = CreateStagingBuffer(roundedSize);
    return VulkanStagingBufferAllocation(stagingBuffer, roundedSize);
}

void VulkanStagingBufferAllocator::FreeBuffer(VulkanStagingBufferAllocation &bufferAllocation) {
    if (!bufferAllocation.IsValid())
        return;

    auto bufferPoolIterator = m_bufferPool.find(bufferAllocation.m_stagingBufferSize);
    if (bufferPoolIterator == m_bufferPool.end())
        return;

    bufferPoolIterator->second.emplace(bufferAllocation.m_stagingBuffer);
    bufferAllocation.Invalidate();
}

VulkanBuffer* VulkanStagingBufferAllocator::CreateStagingBuffer(vk::DeviceSize bufferSize) const {
    VulkanBufferCreateInfo createInfo{};
    createInfo.m_vulkanDevice = m_vulkanDevice;
    createInfo.m_size = bufferSize;
    createInfo.m_usageFlags = vk::BufferUsageFlagBits::eTransferSrc;
    createInfo.m_memoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    createInfo.m_dedicatedRequirements = DedicatedMemoryRequirementsFlags::UseDedicatedIfPreferred;
    InOutCreateParams<Photon::Result> inOut{};
    VulkanBuffer* buffer = VulkanBuffer::Create(createInfo, &inOut);
    if (inOut.m_result != Photon::Result::Success) {
        delete buffer;
        return nullptr;
    }

    return buffer;
}
