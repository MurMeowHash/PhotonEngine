#include "../../Public/Resources/VulkanRHLBuffer.h"

#include "CoreUtils.h"

VulkanRHLBuffer* VulkanRHLBuffer::Create(const VulkanRHLBufferCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.size = createInfo.m_bufferSize;
    bufferCreateInfo.usage =
}

vk::BufferUsageFlags VulkanRHLBuffer::GetVulkanUsageFromPhoton(VulkanRHLBufferUsage usage) {
    vk::BufferUsageFlags vulkanUsageFlags{};

    if (IS_FLAG_SET(VulkanRHLBufferUsage::Static, usage))
        vulkanUsageFlags |= vk::BufferUsageFlagBits::eTransferDst;

    if (IS_FLAG_SET(VulkanRHLBufferUsage::VertexBuffer, usage))
        vulkanUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
}