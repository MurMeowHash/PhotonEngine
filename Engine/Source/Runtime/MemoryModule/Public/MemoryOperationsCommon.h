#pragma once

namespace Rat::MemoryOperationsCommon {
    template<typename T>
    T AlignForward(T ptr, T alignment) {
        return (ptr + (alignment - 1)) & ~(alignment - 1);
    }
}