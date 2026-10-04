#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanImageViewDescriptor.h"

class VulkanDevice;
class VulkanRHLTexture;

struct VulkanImageViewCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanRHLTexture* m_vulkanImage;
    VulkanImageViewDescriptor m_desc;
};

class VulkanImageView {
public:
    static VulkanImageView* Create(const VulkanImageViewCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
private:
    vk::raii::ImageView m_handle = nullptr;
};