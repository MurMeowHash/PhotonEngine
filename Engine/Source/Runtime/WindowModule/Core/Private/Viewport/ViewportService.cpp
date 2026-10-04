#include "../../Public/Viewport/ViewportService.h"
#include "Math.h"
#include "ModuleGlobals.h"
#include "VulkanRHLModule.h"
#include "Window.h"
#include "RenderData/RenderTarget.h"

using Photon::Module::g_activeModuleSequence;

ViewportService::~ViewportService() {
    delete m_vulkanRHLViewport;
}

ViewportService* ViewportService::Create(const ViewportServiceCreateInfo& createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanRHLModule* vulkanRHLModule;
    if (!g_activeModuleSequence->TryResolveModule<VulkanRHLModule>(vulkanRHLModule)) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    RHLViewportCreateInfo rhlViewportCreateInfo{};
    rhlViewportCreateInfo.m_windowHandle = createInfo.m_window->GetHandle();
    rhlViewportCreateInfo.m_viewportWidth = createInfo.m_window->GetWidth();
    rhlViewportCreateInfo.m_viewportHeight = createInfo.m_window->GetHeight();
    rhlViewportCreateInfo.m_blockUntilVBlank = false;
    InOutCreateParams<Photon::Result> rhlViewportInOutCreateParams{};
    VulkanRHLViewport* rhlViewport = vulkanRHLModule->GetVulkanDynamicRHL()->CreateVulkanRHLViewport(rhlViewportCreateInfo, &rhlViewportInOutCreateParams);
    if (rhlViewportInOutCreateParams.m_result != Photon::Result::Success) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    ViewportService* instance = Photon::AllocateObject<ViewportService>(inOutCreateParams);
    instance->m_vulkanRHLViewport = rhlViewport;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

Photon::Result ViewportService::AddViewport(const RenderRect &renderRect) {
    AABB2D candidAABB(renderRect.m_xMin, renderRect.m_xMax, renderRect.m_yMin, renderRect.m_yMax);
    for (const Viewport& viewport: m_viewports) {
        RenderRect viewportRect = viewport.GetRenderRect();
        AABB2D existingAABB(viewportRect.m_xMin, viewportRect.m_xMax, viewportRect.m_yMin, viewportRect.m_yMax);
        if (Photon::Math::Intersect(candidAABB, existingAABB) > 0)
            return Photon::Result::UnknownFailure;
    }

    m_viewports.emplace_back(renderRect, m_vulkanRHLViewport);
    return Photon::Result::Success;
}