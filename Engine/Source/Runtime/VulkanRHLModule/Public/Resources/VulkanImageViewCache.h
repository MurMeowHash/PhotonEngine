#pragma once
#include "VulkanImageView.h"
#include "VulkanImageViewDescriptor.h"
#include "Blueprints/BlueprintProvider.h"

class VulkanDevice;
class VulkanRHLTexture;

class VulkanImageViewCache : public BlueprintProvider<VulkanImageView, VulkanImageViewDescriptor> {
public:
    VulkanImageViewCache(VulkanDevice* vulkanDevice, VulkanRHLTexture* vulkanImage);

protected:
    VulkanImageView* CreateBlueprint(const VulkanImageViewDescriptor &desc, void *allocatedMemory) override;

private:
    VulkanDevice* m_vulkanDevice;
    VulkanRHLTexture* m_vulkanImage;
};
