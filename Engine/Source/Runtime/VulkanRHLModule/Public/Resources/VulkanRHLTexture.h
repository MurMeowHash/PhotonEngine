#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Resources/VulkanImageViewCache.h"
#include "VulkanGlobals.h"
#include "VulkanRHLResource.h"
#include "Resources/VulkanResourceTransition.h"

class VulkanDevice;

struct VulkanImageCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::Image m_imageHandle;
    Photon::Vulkan::RHLFormat m_imageFormat;
    Photon::Vulkan::ImageType m_imageType;
    uint32_t m_arrayLayers;
    uint32_t m_mipLevels;
};

class VulkanRHLTexture : public VulkanRHLResource {
public:
    void CommitPendingTransitionScope() override;

public:
    ~VulkanRHLTexture() override;
    static VulkanRHLTexture* Create(const VulkanImageCreateInfo& createInfo);
    [[nodiscard]] vk::Image GetHandle() const;
    [[nodiscard]] VulkanImageView* GetDefaultImageView() const;
    [[nodiscard]] Photon::Vulkan::RHLFormat GetFormat() const;
    void TrackLastTransitionScope(const VulkanPipelineImageTransitionScope& transitionScope);
    [[nodiscard]] vk::ImageLayout GetLastLayout() const;
private:
    vk::Image m_handle;
    Photon::Vulkan::RHLFormat m_imageFormat{};
    vk::Format m_nativeImageFormat{};
    Photon::Vulkan::ImageType m_imageType{};
    uint32_t m_arrayLayers{};
    uint32_t m_mipLevels{};
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanImageViewCache* m_imageViewCache = nullptr;
    VulkanImageViewDescriptor m_defaultImageViewDescriptor{};
    vk::ImageLayout m_lastTransitionLayout{};
    VulkanPipelineImageTransitionScope m_pendingTransitionScope{};

    bool m_isRaii = true;

    void CreateImageViewCache();
    void CreateDefaultImageViewDescriptor();
    [[nodiscard]] vk::ImageViewType GetDefaultImageViewType() const;
    [[nodiscard]] vk::ImageAspectFlags GetDefaultImageAspectFlags() const;
};