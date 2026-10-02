#pragma once
#include <unordered_map>
#include <windows.h>
#include "Window.h"

namespace Photon::Window {
    inline std::unordered_map<HWND, class Window> g_nativeToEngineWindowMap;
}
