#pragma once
#include <vector>
#include "ModuleBlueprint.h"
#include "WindowModule.h"
#include "DebugModule.h"
#include "VulkanRHLModule.h"
#include "PhotonGlobals.h"

#if PHOTON_ENVIRONMENT_IS_GAME
#include "GameEngineModule.h"
#else
#include "MockEngineModule.h"
#endif

namespace Photon::Module {
    inline std::vector<ModuleBlueprint> g_prioritizedModuleSequence = {
        ModuleBlueprint::Bind<DebugModule>().To<DebugModule>().Finalize(),
        ModuleBlueprint::Bind<WindowModule>().To<WindowModule>().Finalize(),
        ModuleBlueprint::Bind<VulkanRHLModule>().To<VulkanRHLModule>().Finalize(),
#if PHOTON_ENVIRONMENT_IS_GAME
        ModuleBlueprint::Bind<GameEngineModule>().To<GameEngineModule>().To<EngineModule>().Finalize(),
#else
        ModuleBlueprint::Bind<MockEngineModule>().To<MockEngineModule>().To<EngineModule>().Finalize(),
#endif
    };
}