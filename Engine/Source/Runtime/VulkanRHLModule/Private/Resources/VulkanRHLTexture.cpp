#include "../../Public/Resources/VulkanRHLTexture.h"
#include "Device/VulkanDevice.h"

VulkanRHLTexture::~VulkanRHLTexture() {
    delete m_imageViewCache;

    if (m_isRaii) {
        (*m_vulkanDevice->GetHandle()).destroyImage(m_handle);
    }
}

VulkanRHLTexture* VulkanRHLTexture::Create(const VulkanImageCreateInfo& createInfo) {
    VulkanRHLTexture* instance = Photon::AllocateObject<VulkanRHLTexture, Photon::Result>(nullptr);
     instance->m_isRaii = false;
     instance->m_vulkanDevice = createInfo.m_vulkanDevice;
     instance->m_handle = createInfo.m_imageHandle;
     instance->m_imageFormat = createInfo.m_imageFormat;
     instance->m_imageType = createInfo.m_imageType;
     instance->m_arrayLayers = createInfo.m_arrayLayers;
     instance->m_mipLevels = createInfo.m_mipLevels;
     instance->CreateImageViewCache();
     instance->CreateDefaultImageViewDescriptor();
    return  instance;
}

vk::Image VulkanRHLTexture::GetHandle() const {
    return m_handle;
}

VulkanImageView* VulkanRHLTexture::GetDefaultImageView() const {
    return m_imageViewCache->PoolBlueprint(m_defaultImageViewDescriptor);
}

void VulkanRHLTexture::CreateImageViewCache() {
    m_imageViewCache = new VulkanImageViewCache(m_vulkanDevice, this);
}

void VulkanRHLTexture::CreateDefaultImageViewDescriptor() {
    m_defaultImageViewDescriptor.m_viewType = GetDefaultImageViewType();
    m_defaultImageViewDescriptor.m_format = m_imageFormat;
    m_defaultImageViewDescriptor.m_components = vk::ComponentMapping(
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity
        );
    m_defaultImageViewDescriptor.m_subresourceRange = vk::ImageSubresourceRange(
        GetDefaultImageAspectFlags(),
        0,
        m_mipLevels,
        0,
        m_arrayLayers
        );
}

vk::ImageViewType VulkanRHLTexture::GetDefaultImageViewType() const {
    bool hasLayers = m_arrayLayers > 1;
    switch (m_imageType) {
        case Photon::Vulkan::ImageType::p1D:
            return hasLayers ? vk::ImageViewType::e1DArray : vk::ImageViewType::e1D;
            break;
        case Photon::Vulkan::ImageType::p2D:
            return hasLayers ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D;
            break;
        case Photon::Vulkan::ImageType::p3D:
            return vk::ImageViewType::e3D;
            break;
        case Photon::Vulkan::ImageType::pCubeMap:
            return hasLayers ? vk::ImageViewType::eCube : vk::ImageViewType::eCubeArray;
            break;
        default:
            throw std::runtime_error(StringFormatter("Failed to get default image view type of type, ", static_cast<uint32_t>(m_imageType)));
    }
}

vk::ImageAspectFlags VulkanRHLTexture::GetDefaultImageAspectFlags() const {
    switch (m_imageFormat){
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD32SfloatS8Uint:
            return vk::ImageAspectFlagBits::eDepth |
                   vk::ImageAspectFlagBits::eStencil;

        case vk::Format::eD16Unorm:
        case vk::Format::eX8D24UnormPack32:
        case vk::Format::eD32Sfloat:
            return vk::ImageAspectFlagBits::eDepth;

        case vk::Format::eS8Uint:
            return vk::ImageAspectFlagBits::eStencil;

        default:
            return vk::ImageAspectFlagBits::eColor;
    }
}