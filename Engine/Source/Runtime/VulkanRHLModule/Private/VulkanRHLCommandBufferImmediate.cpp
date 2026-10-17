#include "../Public/VulkanRHLCommandBufferImmediate.h"
#include "Viewport/VulkanRHLViewport.h"

VulkanRHLCommandBufferImmediate* VulkanRHLCommandBufferImmediate::Create([[maybe_unused]] const VulkanRHLCommandBufferImmediateCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    return VulkanRHLCommandBuffer::Create<VulkanRHLCommandBufferImmediate>({}, inOutCreateParams);
}

bool VulkanRHLCommandBufferImmediate::BeginRenderingViewport(VulkanRHLViewport *rhlViewport) {
    return rhlViewport->AdvanceBackBuffer(m_rhlGraphicsWorkContext);
}

void VulkanRHLCommandBufferImmediate::EndRenderingViewport(VulkanRHLViewport *rhlViewport) {
    rhlViewport->Present(m_rhlGraphicsWorkContext);
}