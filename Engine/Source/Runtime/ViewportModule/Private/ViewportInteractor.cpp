#include "../Public/ViewportInteractor.h"
#include "RendererFactory.h"
#include "Viewport.h"

void ViewportInteractor::ConnectViewport(Viewport *viewport) {
    m_viewport = viewport;
}

void ViewportInteractor::RenderViewport() const {
    m_viewport->BeginRendering();
    std::unique_ptr<IRenderer> renderer = Photon::RendererFactory::CreateRenderer(RendererType::Forward);
    RenderSceneInput renderSceneInput{};
    renderSceneInput.m_renderTarget = m_viewport;
    RenderViewCollection renderViewCollection{};
    renderViewCollection.m_renderViews.emplace_back(RenderRect(0, m_viewport->GetWidth(), 0, m_viewport->GetHeight()));
    renderSceneInput.m_renderViewCollection = renderViewCollection;
    renderer->Render(renderSceneInput);
    m_viewport->EndRendering();
}

void ViewportInteractor::ChangeViewportSize(uint32_t windowWidth, uint32_t windowHeight) {
    m_viewport->ChangeSize(windowHeight, windowWidth);
}