#include "CoreLoop.h"
#include "EngineLoopGlobals.h"

using Photon::EngineLoops::g_coreLoop;

int main() {
    bool errorClean = g_coreLoop->Initialize();

    while(errorClean) {
        errorClean = g_coreLoop->Tick();
    }

    g_coreLoop->Exit();
    return 0;
}