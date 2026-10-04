#pragma once
#include "RenderData/RenderTarget.h"

class VulkanRHLViewport;

class Viewport : public RenderTarget {
public:
    VulkanRHLTexture* GetVulkanRHLTexture() override;
public:
    Viewport(const RenderRect& renderRect, VulkanRHLViewport* vulkanRHLViewport);
private:
    VulkanRHLViewport* m_vulkanRHLViewport = nullptr;
};
