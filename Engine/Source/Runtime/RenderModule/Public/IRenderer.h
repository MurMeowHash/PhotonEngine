#pragma once
#include "RenderData/RenderSceneInput.h"

class VulkanRHLCommandBuffer;

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void Render(const RenderSceneInput& renderSceneInput, VulkanRHLCommandBuffer* rhlCommandBuffer) = 0;
};