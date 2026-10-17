#pragma once

#include <filesystem>

namespace Photon::Core::PlatformInteractor {
    void RequestExit(bool forced);
    [[nodiscard]] std::filesystem::path GetExecutableDirectory();
    [[nodiscard]] std::filesystem::path GetEngineDirectory();
}