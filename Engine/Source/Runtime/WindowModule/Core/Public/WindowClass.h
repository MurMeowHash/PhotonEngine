#pragma once

#include <windows.h>
#include "WindowClassDescriptor.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

struct WindowClassCreateInfo {
    WindowClassDescriptor m_desc;
};

class WindowClass {
public:
    ~WindowClass();
    static WindowClass* Create(const WindowClassCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] const char* GetNativeName() const;
private:
    ATOM m_handle = 0;
    const char* m_className = "";

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};