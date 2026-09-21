#pragma once

enum class BuildType {
    Development = 0,
    Release = 1,
};

namespace Photon::BuildConfiguration {
    inline BuildType g_buildType = BuildType::Development;
}