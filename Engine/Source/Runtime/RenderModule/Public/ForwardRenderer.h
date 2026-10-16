#pragma once
#include "IRenderer.h"

class VulkanRHLRect;

class ForwardRenderer : public IRenderer {
public:
    void Render(const RenderSceneInput& renderSceneInput, VulkanRHLCommandBuffer* rhlCommandBuffer) override;

private:
    static VulkanRHLRect RenderRectToRHL(const RenderRect& renderRect);
};
