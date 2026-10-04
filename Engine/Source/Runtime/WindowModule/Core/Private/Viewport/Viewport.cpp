#include "../../Public/Viewport/Viewport.h"

VulkanRHLTexture* Viewport::GetVulkanRHLTexture() {
    return nullptr;
}

Viewport::~Viewport() {
    delete m_processor;
}

Viewport::Viewport(const RenderRect &renderRect, VulkanRHLViewport *vulkanRHLViewport)
: RenderTarget(renderRect), m_vulkanRHLViewport(vulkanRHLViewport) {
    m_processor = new ViewportProcessor(this);
}

ViewportProcessor* Viewport::GetProcessor() const {
    return m_processor;
}