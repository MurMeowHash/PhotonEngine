#pragma once
#include "CoreGlobals.h"

namespace Photon::EngineDataConfiguration {
    inline const char* g_engineName = "Photon Engine";
    inline const char* g_applicationName = "Photon Engine Development";
    inline Core::Version g_engineVersion = Core::Version(0, 0, 1);
    inline Core::Version g_applicationVersion = Core::Version(0, 0, 1);
    inline Core::VulkanApiVersion g_vulkanVersion = Core::VulkanApiVersion(0, 1, 3, 0);
}