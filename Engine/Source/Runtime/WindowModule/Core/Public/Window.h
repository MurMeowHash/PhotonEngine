#pragma once

#include <windows.h>
#include <cstdint>
#include "FactoryGlobals.h"
#include "CoreGlobals.h"
#include "WindowModule.h"
#include "IWindowProcessor.h"
#include "ModuleGlobals.h"
#include "Viewport/ViewportService.h"

struct WindowCreateInfo {
    uint32_t m_width;
    uint32_t m_height;
    bool m_fullScreen;
    const char* m_title;
};

class Window {
    friend class WindowClass;
public:
    virtual ~Window();

    template<std::derived_from<Window> TWindow>
    static TWindow* Create(const WindowCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) {
        TWindow* window = Photon::AllocateObject<TWindow, Photon::Result>(inOutCreateParams);
        Photon::Result windowCreateResult = window->CreateInternal(createInfo);
        Photon::PushResult(windowCreateResult, inOutCreateParams);
        return window;
    }

    [[nodiscard]] Photon::Result Show() const;
    [[nodiscard]] Photon::Result Hide() const;
    [[nodiscard]] uint32_t GetWidth() const;
    [[nodiscard]] uint32_t GetHeight() const;
    [[nodiscard]] HWND GetHandle() const;
protected:
    uint32_t m_width{};
    uint32_t m_height{};
    ViewportService* m_viewportService = nullptr;

    virtual IWindowProcessor* CreateWindowProcessor();
    [[nodiscard]] virtual Photon::Result PostInitialize();

private:
    static constexpr const char* GENERIC_WINDOW_CLASS_NAME = "GenericWindowClass";

    HWND m_handle = nullptr;
    IWindowProcessor* m_windowProcessor = nullptr;

    [[nodiscard]] Photon::Result CreateInternal(const WindowCreateInfo& createInfo);
    [[nodiscard]] Photon::Result SetActiveWindow(bool isActive) const;

    void OnWindowCloseRequested();
    void OnWindowResized(uint32_t newWidth, uint32_t newHeight);
};