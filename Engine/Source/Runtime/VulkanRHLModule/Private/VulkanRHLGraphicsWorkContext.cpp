#include "../Public/VulkanRHLGraphicsWorkContext.h"
#include "VulkanDynamicRHL.h"
#include "VulkanGlobals.h"

Photon::Result VulkanRHLGraphicsWorkContext::Execute() {
    Photon::Result execResult = VulkanRHLComputeWorkContext::Execute();
    m_pendingGFXState.Invalidate(); //TODO: end frame needs reset()
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
    m_pendingGFXState.SetPipeline(m_vulkanDevice->GetGFXPipeline(pipelineDesc));
}

void VulkanRHLGraphicsWorkContext::DrawPrimitive(uint32_t vertexCount) {
    m_pendingGFXState.Commit(this);
    GetCommandBuffer()->GetHandle().draw(vertexCount, 1, 0, 0);
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
    return vk::Rect2D(vk::Offset2D(rhlRect.m_minX, rhlRect.m_minY), vk::Extent2D(rhlRect.m_maxX, rhlRect.m_maxY));
}
