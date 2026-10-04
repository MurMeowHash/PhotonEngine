#include "../../Public/Viewport/Viewport.h"

VulkanRHLTexture* Viewport::GetVulkanRHLTexture() {
    return nullptr;
}

Viewport::Viewport(const RenderRect &renderRect, VulkanRHLViewport *vulkanRHLViewport)
: RenderTarget(renderRect), m_vulkanRHLViewport(vulkanRHLViewport) { }
