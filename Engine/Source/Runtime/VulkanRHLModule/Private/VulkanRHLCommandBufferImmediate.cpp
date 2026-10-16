#include "../Public/VulkanRHLCommandBufferImmediate.h"
#include "Viewport/VulkanRHLViewport.h"

VulkanRHLCommandBufferImmediate* VulkanRHLCommandBufferImmediate::Create([[maybe_unused]] const VulkanRHLCommandBufferImmediateCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    return VulkanRHLCommandBuffer::Create<VulkanRHLCommandBufferImmediate>({}, inOutCreateParams);
}

void VulkanRHLCommandBufferImmediate::BeginRenderingViewport(VulkanRHLViewport *rhlViewport) {
    rhlViewport->AdvanceBackBuffer(m_rhlGraphicsWorkContext);
}

void VulkanRHLCommandBufferImmediate::EndRenderingViewport(VulkanRHLViewport *rhlViewport) {
    rhlViewport->Present(m_rhlGraphicsWorkContext);
}