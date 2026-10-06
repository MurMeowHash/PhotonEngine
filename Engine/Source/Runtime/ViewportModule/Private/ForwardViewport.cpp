#include "../Public/ForwardViewport.h"
#include "VulkanRHLModule.h"
#include "ModuleGlobals.h"

using Photon::Module::g_activeModuleSequence;

ForwardViewport::~ForwardViewport() {
    delete m_vulkanRHLViewport;
}

ForwardViewport* ForwardViewport::Create(const ForwardViewportCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLModule* vulkanRHLModule;
    if (!g_activeModuleSequence->TryResolveModule<VulkanRHLModule>(vulkanRHLModule)) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    RHLViewportCreateInfo rhlViewportCreateInfo{};
    rhlViewportCreateInfo.m_windowHandle = createInfo.m_viewportWindowHandle;
    rhlViewportCreateInfo.m_viewportWidth = createInfo.m_viewportWidth;
    rhlViewportCreateInfo.m_viewportHeight = createInfo.m_viewportHeight;
    rhlViewportCreateInfo.m_blockUntilVBlank = false;
    InOutCreateParams<Photon::Result> rhlViewportInOutCreateParams{};
    VulkanRHLViewport* rhlViewport = vulkanRHLModule->GetVulkanDynamicRHL()->CreateVulkanRHLViewport(rhlViewportCreateInfo, &rhlViewportInOutCreateParams);
    if (rhlViewportInOutCreateParams.m_result != Photon::Result::Success) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    ForwardViewport* instance = Photon::AllocateObject<ForwardViewport>(inOutCreateParams);
    instance->m_vulkanRHLViewport = rhlViewport;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanRHLTexture* ForwardViewport::GetVulkanRHLTexture() {
    return nullptr;
}