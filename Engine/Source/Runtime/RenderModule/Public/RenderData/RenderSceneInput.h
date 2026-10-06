#pragma once

#include "RenderTarget.h"
#include <vector>

struct RenderRect {
    float m_xMin;
    float m_xMax;
    float m_yMin;
    float m_yMax;
};

struct RenderView {
    RenderRect m_renderRect;
};

struct RenderViewCollection {
    std::vector<RenderView> m_renderViews;
};

struct RenderSceneInput {
    RenderTarget* m_renderTarget;
    RenderViewCollection m_renderViewCollection;
};
