#include "../Public/CoreLoop.h"
#include "GlfwWindowProvider.h"
#include "WindowCreateInfo.h"

bool CoreLoop::Initialize() {
    Photon::Window::WindowCreateInfo windowCreateInfo(1920, 1200, false, "Photon Engine");
    if (!Photon::Window::Glfw::CreateMainWindow(windowCreateInfo))
        return false;

    return true;
}

bool CoreLoop::Tick() {
    Photon::Window::Glfw::Tick();
    return true;
}

bool CoreLoop::Exit() {
    Photon::Window::Glfw::Shutdown();
    return true;
}