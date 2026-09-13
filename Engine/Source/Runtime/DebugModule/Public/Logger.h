#pragma once
#include <ostream>

namespace Photon::Logger {
    void SetOutputStream(std::ostream *outputStream);
    void PrintError(const std::string &msg);
    void PrintWarning(const std::string &msg);
    void PrintInfo(const std::string &msg);
}
