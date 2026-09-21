#include "EngineLoopGlobals.h"
#include "CoreLoop.h"
#include "VulkanRHLLoop.h"

namespace Photon::EngineLoops {
    std::shared_ptr<IEngineLoop> g_coreLoop = std::make_shared<CoreLoop>();
    std::shared_ptr<IEngineLoop> g_vulkanRHLLoop = std::make_shared<VulkanRHLLoop>();
}