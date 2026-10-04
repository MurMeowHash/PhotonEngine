#pragma once

class VulkanRHLTexture;

struct RenderRect {
    float m_xMin;
    float m_xMax;
    float m_yMin;
    float m_yMax;
};

class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    explicit RenderTarget(const RenderRect& renderRect);
    virtual VulkanRHLTexture* GetVulkanRHLTexture() = 0;
    [[nodiscard]] RenderRect GetRenderRect() const;
    void ChangeRenderRect(const RenderRect& renderRect);
private:
    RenderRect m_renderRect{};
};