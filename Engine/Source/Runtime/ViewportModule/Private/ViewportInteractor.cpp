#include "../Public/ViewportInteractor.h"
#include "RendererFactory.h"
#include "Viewport.h"
#include "VulkanRHLCommandBufferImmediate.h"
#include "VulkanRHLModule.h"
#include "Shaders/DummyTriangleShader.h"
#include "Shaders/Shader.h"

void ViewportInteractor::ConnectViewport(Viewport *viewport) {
    m_viewport = viewport;
}

void ViewportInteractor::RenderViewport(VulkanRHLCommandBufferImmediate* rhlCommandBuffer) const {
    bool canRender = m_viewport->BeginRendering(rhlCommandBuffer);
    if (!canRender)
        return;

    std::unique_ptr<IRenderer> renderer = Photon::RendererFactory::CreateRenderer(RendererType::Forward);
    RenderSceneInput renderSceneInput{};
    renderSceneInput.m_renderTarget = m_viewport;
    renderSceneInput.m_renderViewCollection.m_renderView = RenderView(RenderRect(0, static_cast<float>(m_viewport->GetWidth()),
        0, static_cast<float>(m_viewport->GetHeight())));
    renderSceneInput.m_renderScene.m_renderObject = RenderObject(GET_GLOBAL_SHADER(DummyTriangleVS),
        GET_GLOBAL_SHADER(DummyTriangleFS), Photon::Vulkan::RHLPrimitiveType::Triangle);
    renderer->Render(renderSceneInput, rhlCommandBuffer);
    m_viewport->EndRendering(rhlCommandBuffer);
}

void ViewportInteractor::ChangeViewportSize(uint32_t windowWidth, uint32_t windowHeight) {
    m_viewport->ChangeSize(windowWidth, windowHeight);
}