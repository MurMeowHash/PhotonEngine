#include "../Public/WindowEventDispatcher.h"
#include <windows.h>

void WindowEventDispatcher::DispatchEvents() {
    MSG windowMsg;
    while (PeekMessageA(&windowMsg, nullptr, 0, 0, PM_REMOVE)) {
        DispatchMessageA(&windowMsg);
    }
}