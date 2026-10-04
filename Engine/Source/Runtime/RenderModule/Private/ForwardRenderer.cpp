#include "../Public/ForwardRenderer.h"
#include "CoreUtils.h"
#include "Logger.h"

void ForwardRenderer::Render(const RenderSceneInput& renderSceneInput) {
    Photon::Logger::PrintInfo(StringFormatter("Forward rendering with target: ", renderSceneInput.m_renderTarget, '\n'));
}