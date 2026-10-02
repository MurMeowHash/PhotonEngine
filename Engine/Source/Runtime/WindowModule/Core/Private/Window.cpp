#include "../Public/Window.h"

#include "DefaultWindowProcessor.h"
#include "WindowModule.h"
#include "ModuleSequence.h"
#include "ModuleGlobals.h"

using Photon::Module::g_activeModuleSequence;

Window::~Window() {
    delete m_windowProcessor;
    DestroyWindow(m_handle);
}

Photon::Result Window::Show() const {
    return SetActiveWindow(true);
}

Photon::Result Window::Hide() const {
    return SetActiveWindow(false);
}

IWindowProcessor * Window::CreateWindowProcessor() {
    return new DefaultWindowProcessor();
}

Photon::Result Window::SetActiveWindow(bool isActive) const {
    if (m_handle == nullptr)
        return Photon::Result::UnknownFailure;

    ShowWindow(m_handle, isActive ? SW_SHOW : SW_HIDE);
    return Photon::Result::Success;
}