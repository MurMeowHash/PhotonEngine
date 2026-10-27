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

    const RenderObject& renderObject = renderSceneInput.m_renderScene.m_renderObject;
    VulkanRHLGraphicsPipelineDescriptor desc{};
    desc.AddVertexStage(VulkanRHLVertexShadingStage()
            .SetVertexLayout(renderObject.m_vertexInput->GetVertexLayout())
            .SetVertexShader(renderObject.m_vertexShader))
        .AddPrimitiveAssemblyStage(VulkanRHLPrimitiveAssemblyStage()
            .SetPrimitiveType(renderObject.m_primitiveType))
        .AddFragmentStage(VulkanRHLFragmentShadingStage()
            .SetFragmentShader(renderObject.m_fragmentShader))
        .AddPresentationStage(VulkanRHLPresentationStage()
            .AddColorAttachment(rhlTextureTarget->GetFormat()));

    rhlCommandBuffer->BindPipeline(desc);
    rhlCommandBuffer->BindVertexInput(renderObject.m_vertexInput);
    rhlCommandBuffer->DrawMeshIndexed(renderObject.m_indexBuffer, renderObject.m_indexCount);

    rhlCommandBuffer->EndRenderPass();
}

VulkanRHLRect ForwardRenderer::RenderRectToRHL(const RenderRect &renderRect) {
    return VulkanRHLRect(static_cast<uint32_t>(renderRect.m_xMin), static_cast<uint32_t>(renderRect.m_yMin),
        static_cast<uint32_t>(renderRect.m_xMax), static_cast<uint32_t>(renderRect.m_yMax));
}