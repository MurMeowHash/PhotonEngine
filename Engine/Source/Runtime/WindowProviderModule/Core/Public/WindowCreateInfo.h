#pragma once

namespace Photon::Window {
    struct WindowCreateInfo {
        int m_width;
        int m_height;
        bool m_fullScreen;
        const char *m_title;
    };
}