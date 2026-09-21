#include "../Public/CoreLoop.h"
#include "EngineLoopGlobals.h"
#include "GlfwWindowProvider.h"
#include "RendererFactory.h"
#include "WindowCreateInfo.h"

using Photon::EngineLoops::g_vulkanRHLLoop;

bool CoreLoop::Initialize() {
    Photon::Window::WindowCreateInfo windowCreateInfo(1920, 1200, false, "Photon Engine"); // TODO: configuration can handle it
    if (!Photon::Window::Glfw::CreateMainWindow(windowCreateInfo))
        return false;

    if (!g_vulkanRHLLoop->Initialize())
        return false;

    return true;
}

bool CoreLoop::Tick() {
    Photon::Window::Glfw::Tick();
    bool rhlLoopValidTick = g_vulkanRHLLoop->Tick();
    if (!rhlLoopValidTick)
        return false;

    IRenderer* renderer = Photon::RendererFactory::CreateRenderer(RendererType::Forward);
    renderer->Render();
    delete renderer;

    return true;
}

bool CoreLoop::Exit() {
    g_vulkanRHLLoop->Exit();
    Photon::Window::Glfw::Shutdown();
    return true;
}