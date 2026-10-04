#include "../../Public/Viewport/ViewportProcessor.h"
#include "RendererFactory.h"
#include "Viewport/Viewport.h"

ViewportProcessor::ViewportProcessor(Viewport *viewport)
: m_viewport(viewport) { }

void ViewportProcessor::RedrawViewport() const {
    IRenderer* renderer = Photon::RendererFactory::CreateRenderer(RendererType::Forward);
    RenderSceneInput renderSceneInput{};
    renderSceneInput.m_renderTarget = m_viewport;
    renderer->Render(renderSceneInput);
    delete renderer;
}