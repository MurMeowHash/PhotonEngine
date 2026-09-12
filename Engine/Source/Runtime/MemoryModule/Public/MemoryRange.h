#pragma once

#include "Range.h"
#include "MemoryOperationsCommon.h"

template<typename TMemory>
class MemoryRange : public Range<TMemory> {
public:
    MemoryRange(TMemory beginBytes, TMemory endBytes)
    : Range<TMemory>(beginBytes, endBytes) {}

    [[nodiscard]] TMemory GetAlignedSize(TMemory alignment) const {
        return this->GetEnd() - Rat::MemoryOperationsCommon::AlignForward(this->GetBegin(), alignment);
    }

    [[nodiscard]] TMemory GetAlignedBegin(TMemory alignment) const {
        return Rat::MemoryOperationsCommon::AlignForward(this->GetBegin(), alignment);
    }

    void ShrinkForwardAligned(TMemory shift, TMemory alignment) {
        this->ShrinkForward(GetAlignedBegin(alignment) - this->GetBegin() + shift);
    }

    void MarkCoalesced() {
        m_coalesced = true;
    }

    [[nodiscard]] bool IsCoalesced() const {
        return m_coalesced;
    }

private:
    bool m_coalesced = false;
};