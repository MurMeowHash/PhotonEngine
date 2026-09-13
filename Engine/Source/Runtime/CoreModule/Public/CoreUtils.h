#pragma once

#include <string>
#include <sstream>

#define StringFormatter(...) Photon::Core::StringFormatter::FormatString(__VA_ARGS__)

namespace Photon::Core::StringFormatter {
    template<typename... Args>
    std::string FormatString(const Args &... args) {
        std::ostringstream oss;
        ((oss << args), ...);
        return oss.str();
    }
}

namespace Photon::Core::Flags {
    template<typename TFlags>
    inline TFlags operator|(TFlags a, TFlags b) {
        using T = std::underlying_type_t<TFlags>;
        return static_cast<TFlags>(static_cast<T>(a) | static_cast<T>(b));
    }

    template<typename TFlags>
    inline TFlags operator&(TFlags a, TFlags b) {
        using T = std::underlying_type_t<TFlags>;
        return static_cast<TFlags>(static_cast<T>(a) & static_cast<T>(b));
    }

    template<typename TFlags>
    inline TFlags& operator|=(TFlags& a, TFlags b) {
        a = a | b;
        return a;
    }

    template<typename TFlags>
    inline TFlags& operator&=(TFlags& a, TFlags b) {
        a = a & b;
        return a;
    }

    template<typename TFlags>
    inline bool IsFlagSet(TFlags value, TFlags flag) {
        using T = std::underlying_type_t<TFlags>;
        return (static_cast<T>(value) & static_cast<T>(flag)) != 0;
    }
}