#pragma once

#include <vector>
#include "Viewport.h"
#include "FactoryGlobals.h"
#include "CoreGlobals.h"

class Window;
struct RenderRect;

struct ViewportServiceCreateInfo {
    Window* m_window;
};

class ViewportService {
public:
    ~ViewportService();
    static ViewportService* Create(const ViewportServiceCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] Photon::Result AddViewport(const RenderRect& renderRect);
private:
    std::vector<Viewport> m_viewports;
    VulkanRHLViewport* m_vulkanRHLViewport = nullptr;
};