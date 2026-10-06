#include "../Public/CoreLoop.h"
#include "ModuleSequence.h"
#include "EngineModuleGlobals.h"
#include "ModuleGlobals.h"

using Photon::Module::g_activeModuleSequence;

bool CoreLoop::Initialize() {
    g_activeModuleSequence = new ModuleSequence(Photon::Module::g_prioritizedModuleSequence);

    while (g_activeModuleSequence->HasNextStartUpModule()) {
        ModuleBase* module = g_activeModuleSequence->GetNextStartUpModule();
        Photon::Result result = module->StartUp();
        if (result != Photon::Result::Success)
            return false;
    }

    EngineModule* engineModule;
    if (!g_activeModuleSequence->TryResolveModule(engineModule))
        return false;

    m_engine = engineModule->GetEngineFactory()->CreateEngine();
    Photon::Result engineInitializeResult = m_engine->Initialize();
    if (engineInitializeResult != Photon::Result::Success)
        return false;

    return true;
}

bool CoreLoop::Tick() {
    WindowModule* windowModule;
    if (!g_activeModuleSequence->TryResolveModule<WindowModule>(windowModule))
        return false;

    windowModule->GetWindowEventDispatcher()->DispatchEvents();
    Photon::Result engineTickResult = m_engine->Tick();
    if (engineTickResult != Photon::Result::Success)
        return false;

    return true;
}

bool CoreLoop::Exit() {
    Photon::Result engineExitResult = m_engine->Exit();
    delete m_engine;

    while (g_activeModuleSequence->HasNextTerminateModule()) {
        ModuleBase* module = g_activeModuleSequence->GetNextTerminateModule();
        module->Terminate();
    }

    delete g_activeModuleSequence;
    g_activeModuleSequence = nullptr;
    return true;
}