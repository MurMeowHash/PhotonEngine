#include "EngineLoopGlobals.h"
#include "CoreLoop.h"

namespace Photon::EngineLoops {
    std::shared_ptr<IEngineLoop> g_coreLoop = std::make_shared<CoreLoop>();
}