#include "../../Public/Resources/VulkanRHLBuffer.h"

#include "CoreUtils.h"

VulkanRHLBuffer* VulkanRHLBuffer::Create(const VulkanRHLBufferCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::BufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.size = createInfo.m_bufferSize;
    bufferCreateInfo.usage =
}

vk::BufferUsageFlags VulkanRHLBuffer::GetVulkanUsageFromPhoton(VulkanRHLBufferUsage usage) {
    vk::BufferUsageFlags vulkanUsageFlags{};

    if (FlagSet(VulkanRHLBufferUsage::Static, usage))
        vulkanUsageFlags |= vk::BufferUsageFlagBits::eTransferDst;

    if (FlagSet(VulkanRHLBufferUsage::VertexBuffer, usage))
        vulkanUsageFlags |= vk::BufferUsageFlagBits::eVertexBuffer;
}