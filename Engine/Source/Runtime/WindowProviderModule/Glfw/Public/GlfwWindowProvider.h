#pragma once

namespace Photon::Window {
    struct WindowCreateInfo;
}

namespace Photon::Window::Glfw {
    bool CreateMainWindow(const WindowCreateInfo& windowCreateInfo);
    void Tick();
    void Shutdown();
}