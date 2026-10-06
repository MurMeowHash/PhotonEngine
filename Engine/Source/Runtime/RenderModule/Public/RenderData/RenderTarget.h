#pragma once

class VulkanRHLTexture;

class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    virtual VulkanRHLTexture* GetVulkanRHLTexture() = 0;
};