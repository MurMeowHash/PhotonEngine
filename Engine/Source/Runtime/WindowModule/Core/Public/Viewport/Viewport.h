#pragma once
#include "ViewportProcessor.h"
#include "RenderData/RenderTarget.h"

class VulkanRHLViewport;

class Viewport : public RenderTarget {
public:
    VulkanRHLTexture* GetVulkanRHLTexture() override;
public:
    ~Viewport() override;
    Viewport(const RenderRect& renderRect, VulkanRHLViewport* vulkanRHLViewport);
    [[nodiscard]] ViewportProcessor* GetProcessor() const;
private:
    VulkanRHLViewport* m_vulkanRHLViewport = nullptr;
    ViewportProcessor* m_processor = nullptr;
};