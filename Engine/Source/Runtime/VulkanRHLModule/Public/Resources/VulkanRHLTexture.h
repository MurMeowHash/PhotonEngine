#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Resources/VulkanImageViewCache.h"
#include "VulkanGlobals.h"

class VulkanDevice;

struct VulkanImageCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::Image m_imageHandle;
    vk::Format m_imageFormat;
    Photon::Vulkan::ImageType m_imageType;
    uint32_t m_arrayLayers;
    uint32_t m_mipLevels;
};

class VulkanRHLTexture {
public:
    ~VulkanRHLTexture();
    static VulkanRHLTexture* Create(const VulkanImageCreateInfo& createInfo);
    [[nodiscard]] vk::Image GetHandle() const;
    [[nodiscard]] VulkanImageView* GetDefaultImageView() const;
private:
    vk::Image m_handle;
    vk::Format m_imageFormat{};
    Photon::Vulkan::ImageType m_imageType{};
    uint32_t m_arrayLayers{};
    uint32_t m_mipLevels{};
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanImageViewCache* m_imageViewCache = nullptr;
    VulkanImageViewDescriptor m_defaultImageViewDescriptor{};

    bool m_isRaii = true;

    void CreateImageViewCache();
    void CreateDefaultImageViewDescriptor();
    [[nodiscard]] vk::ImageViewType GetDefaultImageViewType() const;
    [[nodiscard]] vk::ImageAspectFlags GetDefaultImageAspectFlags() const;
};