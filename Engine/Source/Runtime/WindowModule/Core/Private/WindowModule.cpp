#include "../Public/WindowModule.h"
#include <windows.h>

Photon::Result WindowModule::StartUp() {
    m_windowClassProvider = new WindowClassProvider();
    m_windowEventDispatcher = new WindowEventDispatcher();
    SetDPIAwareness();
    return Photon::Result::Success;
}

void WindowModule::Terminate() {
    delete m_windowClassProvider;
    delete m_windowEventDispatcher;
}

WindowClassProvider* WindowModule::GetWindowClassProvider() const {
    return m_windowClassProvider;
}

WindowEventDispatcher* WindowModule::GetWindowEventDispatcher() const {
    return m_windowEventDispatcher;
}

void WindowModule::SetDPIAwareness() {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}