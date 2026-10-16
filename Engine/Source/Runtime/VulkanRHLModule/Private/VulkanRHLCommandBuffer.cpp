#include "../Public/VulkanRHLCommandBuffer.h"
#include "VulkanDynamicRHL.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

VulkanRHLCommandBuffer::~VulkanRHLCommandBuffer() {
    delete m_rhlGraphicsWorkContext;
}

void VulkanRHLCommandBuffer::Reset() {
    m_rhlGraphicsWorkContext->Reset();
}

VulkanRHLGraphicsWorkContext* VulkanRHLCommandBuffer::GetGraphicsContext() const {
    return m_rhlGraphicsWorkContext;
}

void VulkanRHLCommandBuffer::Transition(const VulkanRHLResourceTransitionRequestBatch &transitionRequests) {
    VulkanRHLPendingTransition pendingTransition = g_vulkanDynamicRHL->CreateTransition(transitionRequests);
    if (pendingTransition.IsCrossPipeline()) {
        GetCommandWorkContext(pendingTransition.GetSrcPipeline())->BeginTransition(pendingTransition);
        GetCommandWorkContext(pendingTransition.GetDestPipeline())->EndTransition(pendingTransition);
    } else {
        GetCommandWorkContext(pendingTransition.GetSrcPipeline())->ExecMonoTransition(pendingTransition);
    }
}

void VulkanRHLCommandBuffer::Transition(VulkanRHLTextureTransition &&textureTransition) {
    VulkanRHLResourceTransitionRequestBatch transitionBatch{textureTransition.m_srcPipeline, textureTransition.m_destPipeline};
    transitionBatch.AddTextureTransitionRequest({textureTransition.m_rhlTexture, textureTransition.m_srcPipelineUsage, textureTransition.m_destPipelineUsage});
    Transition(transitionBatch);
}

void VulkanRHLCommandBuffer::SetViewport(const VulkanRHLViewportRect &viewportRect) {
    m_rhlGraphicsWorkContext->SetViewport(viewportRect);
}

void VulkanRHLCommandBuffer::SetScissor(const VulkanRHLRect &scissorRect) {
    m_rhlGraphicsWorkContext->SetScissor(scissorRect);
}

void VulkanRHLCommandBuffer::BindPipeline(const VulkanRHLGraphicsPipelineDescriptor &pipelineDesc) {
    m_rhlGraphicsWorkContext->BindPipeline(pipelineDesc);
}

void VulkanRHLCommandBuffer::DrawPrimitive(uint32_t vertexCount) {
    m_rhlGraphicsWorkContext->DrawPrimitive(vertexCount);
}

void VulkanRHLCommandBuffer::BeginRenderPass(const VulkanRHLRenderPassBeginInfo &passBeginInfo) {
    m_rhlGraphicsWorkContext->BeginRenderPass(passBeginInfo);
}

void VulkanRHLCommandBuffer::EndRenderPass() {
    m_rhlGraphicsWorkContext->EndRenderPass();
}

void VulkanRHLCommandBuffer::AddCommandWorkContext(VulkanRHLCommandWorkContext *workContext) {
    m_commandWorkContexts[workContext->GetRHLPipeline()] = workContext;
}

VulkanRHLCommandWorkContext* VulkanRHLCommandBuffer::GetCommandWorkContext(RHLPipeline pipeline) const {
    auto workContextIterator = m_commandWorkContexts.find(pipeline);
    if (workContextIterator == m_commandWorkContexts.end())
        return nullptr;

    return workContextIterator->second;
}