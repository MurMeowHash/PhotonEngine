#pragma once

namespace Photon::Core {
    inline bool g_engineExitRequested;

    struct Version {
        int m_major;
        int m_minor;
        int m_patch;
    };

    struct VulkanApiVersion {
        int m_versionVariant;
        int m_versionMajor;
        int m_versionMinor;
        int m_versionPatch;
    };

    inline int InvalidIndex = -1;
}