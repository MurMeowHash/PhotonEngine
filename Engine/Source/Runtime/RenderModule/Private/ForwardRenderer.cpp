#include "../Public/ForwardRenderer.h"
#include "VulkanRHLCommandBuffer.h"
#include "Configurations/RenderConfiguration.h"
#include "Resources/VulkanRHLTexture.h"
#include "VulkanGlobals.h"

void ForwardRenderer::Render(const RenderSceneInput& renderSceneInput, VulkanRHLCommandBuffer* rhlCommandBuffer) {
    if (renderSceneInput.m_renderTarget == nullptr)
        return;

    VulkanRHLTexture* rhlTextureTarget = renderSceneInput.m_renderTarget->GetVulkanRHLTexture();
    rhlCommandBuffer->Transition({rhlTextureTarget, RHLPipeline::Graphics, RHLPipelineUsage::DontCare,
        RHLPipeline::Graphics, RHLPipelineUsage::RenderTarget});

    const RenderRect& renderRect = renderSceneInput.m_renderViewCollection.m_renderView.m_renderRect;
    VulkanRHLRect rhlRenderRect = RenderRectToRHL(renderRect);
    VulkanRHLRenderPassBeginInfo beginInfo{};
    beginInfo.m_colorAttachments = {
        VulkanRHLRenderPassColorAttachmentInput{renderSceneInput.m_renderTarget->GetVulkanRHLTexture(), Photon::RenderConfiguration::g_frameClearColor}
    };
    beginInfo.m_renderAffectedRect = rhlRenderRect;
    rhlCommandBuffer->BeginRenderPass(beginInfo);
    rhlCommandBuffer->SetViewport({renderRect.m_xMin, renderRect.m_yMin, 0.0f, renderRect.m_xMax, renderRect.m_yMax, 1.0f});
    rhlCommandBuffer->SetScissor(rhlRenderRect);

    VulkanRHLGraphicsPipelineDescriptor desc{};
    desc.AddVertexStage(VulkanRHLVertexShadingStage()
            .SetVertexLayout(renderSceneInput.m_renderScene.m_renderObject.m_vertexInput.GetVertexLayout())
            .SetVertexShader(renderSceneInput.m_renderScene.m_renderObject.m_vertexShader))
        .AddPrimitiveAssemblyStage(VulkanRHLPrimitiveAssemblyStage()
            .SetPrimitiveType(renderSceneInput.m_renderScene.m_renderObject.m_primitiveType))
        .AddFragmentStage(VulkanRHLFragmentShadingStage()
            .SetFragmentShader(renderSceneInput.m_renderScene.m_renderObject.m_fragmentShader))
        .AddPresentationStage(VulkanRHLPresentationStage()
            .AddColorAttachment(rhlTextureTarget->GetFormat()));
    rhlCommandBuffer->BindPipeline(desc);
    rhlCommandBuffer->BindVertexInput(renderSceneInput.m_renderScene.m_renderObject.m_vertexInput);

    rhlCommandBuffer->DrawPrimitive(3);
    rhlCommandBuffer->EndRenderPass();
}

VulkanRHLRect ForwardRenderer::RenderRectToRHL(const RenderRect &renderRect) {
    return VulkanRHLRect(static_cast<uint32_t>(renderRect.m_xMin), static_cast<uint32_t>(renderRect.m_yMin),
        static_cast<uint32_t>(renderRect.m_xMax), static_cast<uint32_t>(renderRect.m_yMax));
}