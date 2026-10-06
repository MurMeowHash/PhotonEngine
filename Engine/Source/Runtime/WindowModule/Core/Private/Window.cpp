#include "../Public/Window.h"
#include "GenericWindowProcessor.h"
#include "Math.h"
#include "WindowModule.h"
#include "ModuleSequence.h"
#include "ModuleGlobals.h"
#include "Viewport.h"
#include "ViewportInteractor.h"
#include "VulkanRHLModule.h"

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

uint32_t Window::GetWidth() const {
    return m_width;
}

uint32_t Window::GetHeight() const {
    return m_height;
}

HWND Window::GetHandle() const {
    return m_handle;
}

void Window::AttachViewport(ViewportInteractor *viewport) {
    m_viewport = viewport;
}

void Window::UpdateViewports() const {
    if (m_viewport == nullptr)
        return;

    m_viewport->ChangeViewportSize(m_width, m_height);
}

IWindowProcessor* Window::CreateWindowProcessor() {
    return new GenericWindowProcessor();
}

Photon::Result Window::CreateInternal(const WindowCreateInfo &createInfo) {
    WindowModule* windowModule = nullptr;
    if (!g_activeModuleSequence->TryResolveModule<WindowModule>(windowModule))
        return Photon::Result::UnknownFailure;

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
        this
    );

    if (nativeHandle == nullptr)
        return Photon::Result::UnknownFailure;

    m_handle = nativeHandle;
    m_width = createInfo.m_width;
    m_height = createInfo.m_height;
    m_windowProcessor = CreateWindowProcessor();
    m_windowProcessor->SetWindow(this);
    Photon::Result windowResult = Show();
    if (windowResult != Photon::Result::Success)
        return windowResult;

    UpdateWindow(m_handle);

    return windowResult;
}

Photon::Result Window::SetActiveWindow(bool isActive) const {
    if (m_handle == nullptr)
        return Photon::Result::UnknownFailure;

    ShowWindow(m_handle, isActive ? SW_SHOW : SW_HIDE);
    return Photon::Result::Success;
}

void Window::OnWindowCloseRequested() {
    m_windowProcessor->ProcessWindowCloseRequest();
}

void Window::OnWindowResized(uint32_t newWidth, uint32_t newHeight) {
    m_width = newWidth;
    m_height = newHeight;
    m_windowProcessor->ProcessWindowResize(m_width, m_height);
}