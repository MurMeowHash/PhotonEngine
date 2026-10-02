#pragma once

#include "ModuleBase.h"
#include "WindowClassProvider.h"
#include "WindowEventDispatcher.h"

class Window;

class WindowModule : public ModuleBase {
public:
    [[nodiscard]] Photon::Result StartUp() override;
    void Terminate() override;

    [[nodiscard]] WindowClassProvider* GetWindowClassProvider() const;
    [[nodiscard]] WindowEventDispatcher* GetWindowEventDispatcher() const;
private:
    WindowClassProvider* m_windowClassProvider = nullptr;
    WindowEventDispatcher* m_windowEventDispatcher = nullptr;
};