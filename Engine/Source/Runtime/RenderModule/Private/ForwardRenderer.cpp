#include "../Public/ForwardRenderer.h"
#include "CoreUtils.h"
#include "Logger.h"

void ForwardRenderer::Render(const RenderSceneInput& renderSceneInput) {
    if (renderSceneInput.m_renderTarget == nullptr)
        return;

    Photon::Logger::PrintInfo(StringFormatter("Forward rendering with target: ", renderSceneInput.m_renderTarget, '\n'));
}