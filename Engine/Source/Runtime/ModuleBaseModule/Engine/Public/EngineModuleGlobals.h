#pragma once
#include <vector>
#include "ModuleBlueprint.h"
#include "WindowModule.h"
#include "DebugModule.h"

namespace Photon::Module {
    inline std::vector<ModuleBlueprint> g_prioritizedModuleSequence = {
        ModuleBlueprint::Create<DebugModule>(),
        ModuleBlueprint::Create<WindowModule>(),
    };
}