#include "../Public/WindowClass.h"
#include "Window.h"

WindowClass::~WindowClass() {
    UnregisterClassA(m_className, GetModuleHandleA(nullptr));
}

WindowClass* WindowClass::Create(const WindowClassCreateInfo &createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams) {
    WindowClass* windowClass = Photon::AllocateObject<WindowClass>(inOutCreateParams);
    WNDCLASSA classInfo{};
    classInfo.lpszClassName = createInfo.m_desc.m_className;
    classInfo.hInstance = GetModuleHandleA(nullptr);
    classInfo.lpfnWndProc = WindowClass::WindowProc;

    windowClass->m_handle = RegisterClassA(&classInfo);
    windowClass->m_className = createInfo.m_desc.m_className;
    Photon::Result result = windowClass->m_handle == 0 ? Photon::Result::UnknownFailure : Photon::Result::Success;
    Photon::PushResult(result, inOutCreateParams);
    return windowClass;
}

const char* WindowClass::GetNativeName() const {
    return m_className;
}

LRESULT WindowClass::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        CREATESTRUCTA* createStructInfo = reinterpret_cast<CREATESTRUCTA*>(lParam);
        Window* engineWindow = static_cast<Window*>(createStructInfo->lpCreateParams);
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(engineWindow));
    } else {
        LONG_PTR windowEngineData = GetWindowLongPtrA(hwnd, GWLP_USERDATA);
        Window* engineWindow = reinterpret_cast<Window*>(windowEngineData);

        switch (msg) {
            case WM_CLOSE:
                engineWindow->OnWindowCloseRequested();
                break;
            case WM_SIZE:
                engineWindow->OnWindowResized(LOWORD(lParam), HIWORD(lParam));
                break;
        }
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}
