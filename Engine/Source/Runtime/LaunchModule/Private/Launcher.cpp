#include "CoreGlobals.h"
#include "CoreLoop.h"
#include "EngineLoopGlobals.h"

using Photon::EngineLoops::g_coreLoop;

int main() {
    bool errorClean = g_coreLoop->Initialize();

    while(!Photon::Core::g_engineExitRequested && errorClean) {
        errorClean = g_coreLoop->Tick();
    }

    g_coreLoop->Exit();
    return 0;
}