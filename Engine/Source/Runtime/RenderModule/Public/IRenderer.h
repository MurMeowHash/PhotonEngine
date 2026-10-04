#pragma once
#include "RenderData/RenderSceneInput.h"

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void Render(const RenderSceneInput& renderSceneInput) = 0;
};