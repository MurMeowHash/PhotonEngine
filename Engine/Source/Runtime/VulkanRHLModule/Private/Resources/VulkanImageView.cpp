#include "../../Public/Resources/VulkanImageView.h"

#include "Device/VulkanDevice.h"
#include "Resources/VulkanRHLTexture.h"

VulkanImageView* VulkanImageView::Create(const VulkanImageViewCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    vk::ImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.image = createInfo.m_vulkanImage->GetHandle();
    imageViewCreateInfo.viewType = createInfo.m_desc.m_viewType;
    imageViewCreateInfo.format = createInfo.m_desc.m_format;
    imageViewCreateInfo.components = createInfo.m_desc.m_components;
    imageViewCreateInfo.subresourceRange = createInfo.m_desc.m_subresourceRange;
    vk::ResultValue<vk::raii::ImageView> imageViewWrapper = createInfo.m_vulkanDevice->GetHandle().createImageView(imageViewCreateInfo);
    if (imageViewWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanImageView* instance = Photon::AllocateObject<VulkanImageView>(inOutCreateParams);
    instance->m_handle = std::move(imageViewWrapper.value);
    instance->m_subresource = createInfo.m_desc.m_subresourceRange;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

vk::ImageSubresourceRange VulkanImageView::GetSubresourceRange() const {
    return m_subresource;
}