#include "../Public/PlatformInteractor.h"
#include <windows.h>
#include <processthreadsapi.h>

#include "CoreGlobals.h"

namespace Photon::Core::PlatformInteractor {
    void RequestExit(bool forced) {
        if (forced)
            TerminateProcess(GetCurrentProcess(), 1);
        else
            g_engineExitRequested = true;
    }

    std::filesystem::path GetExecutableDirectory() {
        wchar_t executablePath[MAX_PATH];
        GetModuleFileNameW(nullptr, executablePath, MAX_PATH);
        return std::filesystem::path(executablePath).parent_path();
    }

    std::filesystem::path GetEngineDirectory() {
        return std::filesystem::weakly_canonical(GetExecutableDirectory() / "../../../Engine");
    }
}