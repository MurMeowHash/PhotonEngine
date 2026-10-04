#include "../../Public/Resources/VulkanImageViewCache.h"

VulkanImageViewCache::VulkanImageViewCache(VulkanDevice *vulkanDevice, VulkanRHLTexture *vulkanImage)
: m_vulkanDevice(vulkanDevice), m_vulkanImage(vulkanImage) { }

VulkanImageView* VulkanImageViewCache::CreateBlueprint(const VulkanImageViewDescriptor &desc, void *allocatedMemory) {
    VulkanImageViewCreateInfo createInfo;
    createInfo.m_vulkanDevice = m_vulkanDevice;
    createInfo.m_vulkanImage = m_vulkanImage;
    createInfo.m_desc = desc;
    InOutCreateParams<Photon::Result> inOutCreateParams;
    inOutCreateParams.m_preAllocatedMemory = allocatedMemory;
    return VulkanImageView::Create(createInfo, &inOutCreateParams);
}