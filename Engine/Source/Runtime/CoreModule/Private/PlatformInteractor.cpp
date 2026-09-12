#include "../Public/PlatformInteractor.h"
#include <processthreadsapi.h>

#include "CoreGlobals.h"

namespace Photon::Core::PlatformInteractor {
    void RequestExit(bool forced) {
        if (forced)
            TerminateProcess(GetCurrentProcess(), 1);
        else
            g_engineExitRequested = true;
    }
}
