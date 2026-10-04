#include "../../Public/RenderData/RenderTarget.h"

RenderTarget::RenderTarget(const RenderRect &renderRect) {
    ChangeRenderRect(renderRect);
}

RenderRect RenderTarget::GetRenderRect() const {
    return m_renderRect;
}

void RenderTarget::ChangeRenderRect(const RenderRect &renderRect) {
    m_renderRect = renderRect;
}
