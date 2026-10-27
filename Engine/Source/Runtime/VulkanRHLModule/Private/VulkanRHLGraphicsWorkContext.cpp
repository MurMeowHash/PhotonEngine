#include "../Public/VulkanRHLGraphicsWorkContext.h"
#include "VulkanDynamicRHL.h"
#include "VulkanGlobals.h"
#include "Resources/VulkanRHLBuffer.h"
#include "VulkanUtils.h"

void VulkanRHLGraphicsWorkContext::Reset() {
    VulkanRHLComputeWorkContext::Reset();
    m_pendingGFXState.Reset();
}

Photon::Result VulkanRHLGraphicsWorkContext::Execute() {
    Photon::Result execResult = VulkanRHLComputeWorkContext::Execute();
    m_pendingGFXState.Invalidate();
    return execResult;
}

VulkanRHLGraphicsWorkContext* VulkanRHLGraphicsWorkContext::Create([[maybe_unused]] const VulkanRHLGraphicsWorkContextCreateInfo &createInfo,
                                                                   InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLGraphicsWorkContext* instance = Photon::AllocateObject<VulkanRHLGraphicsWorkContext>(inOutCreateParams);
    Photon::Result initializeResult = instance->InitializeContext(RHLPipeline::Graphics);
    Photon::PushResult(initializeResult, inOutCreateParams);
    return instance;
}

void VulkanRHLGraphicsWorkContext::SetViewport(const VulkanRHLViewportRect &viewportRect) {
    m_pendingGFXState.SetViewport(vk::Viewport(viewportRect.m_minX, viewportRect.m_minY, viewportRect.m_maxX,
        viewportRect.m_maxY, viewportRect.m_minZ, viewportRect.m_maxZ));
}

void VulkanRHLGraphicsWorkContext::SetScissor(const VulkanRHLRect &scissorRect) {
    m_pendingGFXState.SetScissor(RHLRectToVulkan(scissorRect));
}

void VulkanRHLGraphicsWorkContext::BindPipeline(const VulkanRHLGraphicsPipelineDescriptor &pipelineDesc) {
    m_pendingGFXState.SetGraphicsPipeline(m_vulkanDevice->GetGFXPipeline(pipelineDesc));
}

void VulkanRHLGraphicsWorkContext::BindVertexInput(VulkanRHLVertexInput* vertexInput) {
    for (const VulkanRHLVertexStream& vertexStream : vertexInput->GetStreams())
        m_pendingGFXState.SetVertexStream(vertexStream.m_streamSource, vertexStream.m_streamBuffer);
}

void VulkanRHLGraphicsWorkContext::DrawPrimitive(uint32_t vertexCount) {
    m_pendingGFXState.Commit(this);
    GetCommandBuffer()->GetHandle().draw(vertexCount, 1, 0, 0);
}

void VulkanRHLGraphicsWorkContext::DrawMeshIndexed(VulkanRHLBuffer* indexBuffer, uint32_t indexCount) {
    m_pendingGFXState.Commit(this);
    VulkanBufferSlice indexBackingHandle = indexBuffer->GetBackingHandle();
    vk::CommandBuffer cmdBuffer = GetCommandBuffer()->GetHandle();
    cmdBuffer.bindIndexBuffer(
        indexBackingHandle.GetBuffer()->GetHandle(),
        indexBackingHandle.GetOffset(),
        Photon::Vulkan::GetIndexType(indexBuffer->GetElementSize())
        );

    cmdBuffer.drawIndexed(indexCount, 1, 0, 0, 0);
}

void VulkanRHLGraphicsWorkContext::BeginRenderPass(const VulkanRHLRenderPassBeginInfo &passBeginInfo) {
    std::vector<vk::RenderingAttachmentInfo> vulkanColorAttachments;
    vulkanColorAttachments.reserve(passBeginInfo.m_colorAttachments.size());

    for (const VulkanRHLRenderPassColorAttachmentInput& colorAttachmentInput : passBeginInfo.m_colorAttachments) {
        vk::RenderingAttachmentInfo attachmentInfo{};
        attachmentInfo.imageView = colorAttachmentInput.m_rhlTexture->GetDefaultImageView()->GetHandle();
        attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        attachmentInfo.clearValue = vk::ClearValue(vk::ClearColorValue(colorAttachmentInput.m_clearColor.r,
            colorAttachmentInput.m_clearColor.g, colorAttachmentInput.m_clearColor.b, colorAttachmentInput.m_clearColor.a));
        vulkanColorAttachments.emplace_back(attachmentInfo);
    }

    vk::RenderingInfo renderingInfo{};
    renderingInfo.renderArea = RHLRectToVulkan(passBeginInfo.m_renderAffectedRect);
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = vulkanColorAttachments.size();
    renderingInfo.pColorAttachments = vulkanColorAttachments.data();
    GetCommandBuffer()->GetHandle().beginRendering(&renderingInfo);
}

void VulkanRHLGraphicsWorkContext::EndRenderPass() {
    GetCommandBuffer()->GetHandle().endRendering();
}

vk::Rect2D VulkanRHLGraphicsWorkContext::RHLRectToVulkan(const VulkanRHLRect &rhlRect) {
    return vk::Rect2D(vk::Offset2D(static_cast<int32_t>(rhlRect.m_minX), static_cast<int32_t>(rhlRect.m_minY)),
        vk::Extent2D(rhlRect.m_maxX, rhlRect.m_maxY));
}
