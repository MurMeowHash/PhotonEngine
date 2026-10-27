#include "../Public/ViewportInteractor.h"
#include "ModuleGlobals.h"
#include "RendererFactory.h"
#include "Viewport.h"
#include "VulkanRHLCommandBufferImmediate.h"
#include "VulkanRHLModule.h"
#include "Shaders/DummyTriangleShader.h"
#include "Shaders/Shader.h"
#include <glm/glm.hpp>

using Photon::Module::g_activeModuleSequence;

ViewportInteractor::~ViewportInteractor() {
    delete m_dummyObjectVertexBuffer;
    delete m_dummyObjectIndexBuffer;
    delete m_dummyObjectVertexInput;
}

ViewportInteractor::ViewportInteractor(VulkanRHLCommandBufferImmediate *rhlCommandBuffer) {
    CreateDummyRenderObject(rhlCommandBuffer);
}

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
    renderSceneInput.m_renderScene.m_renderObject = RenderObject(
        m_dummyObjectVertexInput,
        m_dummyObjectIndexBuffer,
        m_dummyObjectIndexCount,
        GET_GLOBAL_SHADER(DummyTriangleVS),
        GET_GLOBAL_SHADER(DummyTriangleFS),
        Photon::Vulkan::RHLPrimitiveType::Triangle);
    renderer->Render(renderSceneInput, rhlCommandBuffer);
    m_viewport->EndRendering(rhlCommandBuffer);
}

void ViewportInteractor::ChangeViewportSize(uint32_t windowWidth, uint32_t windowHeight) {
    m_viewport->ChangeSize(windowWidth, windowHeight);
}

void ViewportInteractor::CreateDummyRenderObject(VulkanRHLCommandBufferImmediate* rhlCommandBuffer) {
    VulkanRHLModule* rhlModule;
    if (!g_activeModuleSequence->TryResolveModule(rhlModule))
        return;

    VulkanDynamicRHL* vulkanDynamicRHL = rhlModule->GetVulkanDynamicRHL();

    std::array<glm::vec2, 4> dummyVertices = {
        glm::vec2(-0.5f, 0.5f),
        glm::vec2(-0.5f, -0.5f),
        glm::vec2(0.5f, -0.5f),
        glm::vec2(0.5, 0.5f)
    };

    std::array<uint16_t, 6> dummyIndices = {
        0, 1, 3, 1, 2, 3
    };

    const uint32_t dummyVertexDataStride = sizeof(glm::vec2);
    const uint64_t dummyVerticesDataSize = dummyVertexDataStride * dummyVertices.size();
    RHLBufferCreateInfo vertexBufferCreateInfo{dummyVerticesDataSize, dummyVertexDataStride,
        VulkanRHLBufferUpdateFrequency::Static, VulkanRHLBufferUsage::VertexBuffer};
    InOutCreateParams<Photon::Result> vertexBufferInOut{};
    m_dummyObjectVertexBuffer = vulkanDynamicRHL->CreateRHLBuffer(vertexBufferCreateInfo, &vertexBufferInOut);
    if (vertexBufferInOut.m_result != Photon::Result::Success)
        return;

    void* vertexWriteRange = rhlCommandBuffer->BeginWriteBuffer(m_dummyObjectVertexBuffer, 0, dummyVerticesDataSize);
    memcpy(vertexWriteRange, dummyVertices.data(), dummyVerticesDataSize);
    rhlCommandBuffer->EndWriteBuffer(m_dummyObjectVertexBuffer);

    const uint64_t dummyIndicesDataSize = sizeof(uint16_t) * dummyIndices.size();
    RHLBufferCreateInfo indexBufferCreateInfo{dummyIndicesDataSize, sizeof(uint16_t),
        VulkanRHLBufferUpdateFrequency::Static, VulkanRHLBufferUsage::IndexBuffer};
    InOutCreateParams<Photon::Result> indexBufferInOut{};
    m_dummyObjectIndexBuffer = vulkanDynamicRHL->CreateRHLBuffer(indexBufferCreateInfo, &indexBufferInOut);
    if (indexBufferInOut.m_result != Photon::Result::Success)
        return;

    void* indexWriteRange = rhlCommandBuffer->BeginWriteBuffer(m_dummyObjectIndexBuffer, 0, dummyIndicesDataSize);
    memcpy(indexWriteRange, dummyIndices.data(), dummyIndicesDataSize);
    rhlCommandBuffer->EndWriteBuffer(m_dummyObjectIndexBuffer);

    VulkanRHLVertexLayoutDescriptor layoutDescriptor{};
    layoutDescriptor.AddStreamDescription({0, dummyVertexDataStride, VulkanRHLVertexInputRate::PerVertex});
    layoutDescriptor.AddVertexDescription({VulkanRHLShaderSemantic(VulkanRHLShaderSemanticType::Position, 0),
        VulkanRHLFormat::R32G32_Float, 0, 0});

    VulkanRHLVertexStream vertexStream{0, m_dummyObjectVertexBuffer};
    m_dummyObjectVertexInput = rhlModule->GetVulkanDynamicRHL()->CreateVertexInput({layoutDescriptor, {vertexStream}});
    m_dummyObjectIndexCount = dummyIndices.size();
}