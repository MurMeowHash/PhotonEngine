#include "../Public/Logger.h"

namespace Photon::Logger {
    std::ostream *m_outputStream = nullptr;

    void PrintMsg(const std::string &msg) {
        *m_outputStream << msg;
        std::flush(*m_outputStream);
    }

    void SetOutputStream(std::ostream *outputStream) {
        m_outputStream = outputStream;
    }

    void PrintError(const std::string &msg) {
        PrintMsg(msg);
    }

    void PrintWarning(const std::string &msg) {
        PrintMsg(msg);
    }

    void PrintInfo(const std::string &msg) {
        PrintMsg(msg);
    }
}
