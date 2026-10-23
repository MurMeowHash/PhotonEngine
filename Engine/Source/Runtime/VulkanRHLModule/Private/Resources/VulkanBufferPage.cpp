#include "../../Public/Resources/VulkanBufferPage.h"
#include "Resources/VulkanBuffer.h"

void VulkanBufferPage::InitializeMemoryObject(VulkanBufferSlice& memoryObject, const MemoryInitializeInfo<vk::DeviceSize>& memInitializeInfo) {
    VulkanBufferSliceInitializeInfo initializeInfo{};
    initializeInfo.m_buffer = m_memory.GetBuffer();
    initializeInfo.m_memInfo = memInitializeInfo;
    memoryObject.Initialize(initializeInfo);
}

bool VulkanBufferPage::OwnsMemory(const VulkanBufferSlice &memory) const {
    return m_memory.GetBuffer() == memory.GetBuffer();
}

VulkanBufferPage::~VulkanBufferPage() {
    delete m_memory.GetBuffer();
}

VulkanBufferPage::VulkanBufferPage(const VulkanBufferPageCreateInfo &createInfo) {
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

std::unique_ptr<VulkanBufferPage> VulkanBufferPagesAllocator::AllocatePage(vk::DeviceSize pageSize) {
    VulkanBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.m_vulkanDevice = m_vulkanDevice;
    bufferCreateInfo.m_size = pageSize;
    bufferCreateInfo.m_usageFlags = m_usageFlags;
    bufferCreateInfo.m_memoryPropertyFlags = m_memoryPropertyFlags;
    bufferCreateInfo.m_dedicatedRequirements = m_dedicatedRequirements;
    return std::make_unique<VulkanBufferPage>(VulkanBufferPageCreateInfo{bufferCreateInfo});
}

VulkanBufferPagesAllocator::VulkanBufferPagesAllocator(const VulkanBufferPagesAllocatorCreateInfo &createInfo)
: MemoryPagesAllocator(createInfo.m_pageSize, createInfo.m_releaseEmptyPages),
m_vulkanDevice(createInfo.m_vulkanDevice), m_usageFlags(createInfo.m_usageFlags),
m_memoryPropertyFlags(createInfo.m_memoryPropertyFlags), m_dedicatedRequirements(createInfo.m_dedicatedRequirements) { }

std::unique_ptr<VulkanBufferPage> VulkanBufferPagesAllocatorSeparate::AllocatePage(vk::DeviceSize pageSize, const vk::BufferUsageFlags &usageFlags) {
    VulkanBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.m_vulkanDevice = m_vulkanDevice;
    bufferCreateInfo.m_size = pageSize;
    bufferCreateInfo.m_usageFlags = usageFlags;
    bufferCreateInfo.m_memoryPropertyFlags = m_memoryPropertyFlags;
    bufferCreateInfo.m_dedicatedRequirements = m_dedicatedRequirements;
    return std::make_unique<VulkanBufferPage>(VulkanBufferPageCreateInfo{bufferCreateInfo});
}

VulkanBufferPagesAllocatorSeparate::VulkanBufferPagesAllocatorSeparate(const VulkanBufferPagesAllocatorSeparateCreateInfo &createInfo)
: MemoryPagesAllocatorSeparate(), m_vulkanDevice(createInfo.m_vulkanDevice),
m_memoryPropertyFlags(createInfo.m_memoryPropertyFlags), m_dedicatedRequirements(createInfo.m_dedicatedRequirements) { }
