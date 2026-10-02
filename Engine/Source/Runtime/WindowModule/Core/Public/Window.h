#pragma once

#include <windows.h>
#include <cstdint>
#include "FactoryGlobals.h"
#include "CoreGlobals.h"
#include "WindowModule.h"
#include "IWindowProcessor.h"
#include "ModuleGlobals.h"

using Photon::Module::g_activeModuleSequence;

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
        WindowModule* windowModule = nullptr;
        TWindow* window = Photon::AllocateObject<TWindow, Photon::Result>(inOutCreateParams);
        if (!g_activeModuleSequence->TryResolveModule<WindowModule>(windowModule)) {
            Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
            return window;
        }

        WindowClassDescriptor desc;
        desc.m_className = GENERIC_WINDOW_CLASS_NAME;
        WindowClass* windowClass = windowModule->GetWindowClassProvider()->PoolBlueprint(desc);
        HWND nativeHandle = CreateWindowExA(
            0,
            windowClass->GetNativeName(),
            createInfo.m_title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<int>(createInfo.m_width),
            static_cast<int>(createInfo.m_height),
            nullptr,
            nullptr,
            GetModuleHandleA(nullptr),
            window
        );

        if (nativeHandle == nullptr) {
            Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
            return window;
        }

        window->m_handle = nativeHandle;
        window->m_windowProcessor = static_cast<Window*>(window)->CreateWindowProcessor();
        Photon::Result windowShowResult = window->Show();
        UpdateWindow(window->m_handle);

        Photon::PushResult(windowShowResult, inOutCreateParams);
        return window;
    }

    [[nodiscard]] Photon::Result Show() const;
    [[nodiscard]] Photon::Result Hide() const;

protected:
    virtual IWindowProcessor* CreateWindowProcessor();

private:
    static constexpr const char* GENERIC_WINDOW_CLASS_NAME = "GenericWindowClass";

    HWND m_handle = nullptr;
    IWindowProcessor* m_windowProcessor = nullptr;

    [[nodiscard]] Photon::Result SetActiveWindow(bool isActive) const;
};