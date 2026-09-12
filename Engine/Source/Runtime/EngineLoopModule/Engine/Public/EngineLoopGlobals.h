#pragma once

#include <memory>

class IEngineLoop;

namespace Photon::EngineLoops {
    extern std::shared_ptr<IEngineLoop> g_coreLoop;
}
