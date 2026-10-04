#include "../Public/CoreLoop.h"
#include "RendererFactory.h"
#include "ModuleSequence.h"
#include "EngineModuleGlobals.h"
#include "GameWindow.h"
#include "ModuleGlobals.h"
#include "Window.h"
#include "FactoryGlobals.h"

using Photon::Module::g_activeModuleSequence;

bool CoreLoop::Initialize() {
    g_activeModuleSequence = new ModuleSequence(Photon::Module::g_prioritizedModuleSequence);

    while (g_activeModuleSequence->HasNextStartUpModule()) {
        ModuleBase* module = g_activeModuleSequence->GetNextStartUpModule();
        Photon::Result result = module->StartUp();
        if (result != Photon::Result::Success)
            return false;
    }

    if (!CreateGameWindow())
        return false;

    return true;
}

bool CoreLoop::Tick() {
    WindowModule* windowModule;
    if (!g_activeModuleSequence->TryResolveModule<WindowModule>(windowModule))
        return false;

    windowModule->GetWindowEventDispatcher()->DispatchEvents();

    IRenderer* renderer = Photon::RendererFactory::CreateRenderer(RendererType::Forward);
    renderer->Render();
    delete renderer;

    return true;
}

bool CoreLoop::Exit() {
    delete m_gameWindow;

    while (g_activeModuleSequence->HasNextTerminateModule()) {
        ModuleBase* module = g_activeModuleSequence->GetNextTerminateModule();
        module->Terminate();
    }

    delete g_activeModuleSequence;
    g_activeModuleSequence = nullptr;
    return true;
}

bool CoreLoop::CreateGameWindow() {
    WindowCreateInfo createInfo(1920, 1200, false, "Photon Engine"); // TODO: create configuration
    InOutCreateParams<Photon::Result> inOutCreateParams;
    m_gameWindow = Window::Create<GameWindow>(createInfo, &inOutCreateParams);
    return inOutCreateParams.m_result == Photon::Result::Success;
}