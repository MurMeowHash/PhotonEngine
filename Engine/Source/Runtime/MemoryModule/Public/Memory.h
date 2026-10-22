#pragma once

template<typename TSize>
struct MemoryInitializeInfo {
    TSize m_size;
    TSize m_offset;
    TSize m_alignmentOffset;
};

template<typename TSize>
class Memory {
public:
    virtual ~Memory() = default;

    [[nodiscard]] TSize GetSize() const {
        return m_size;
    }

    [[nodiscard]] TSize GetAllocationSize() const {
        return m_size + m_alignmentOffset;
    }

    [[nodiscard]] TSize GetOffset() const {
        return m_offset;
    }

    [[nodiscard]] TSize GetAlignmentOffset() const {
        return m_alignmentOffset;
    }

    [[nodiscard]] TSize GetMemoryOccupationOffset() const {
        return m_offset - m_alignmentOffset;
    }

    virtual void InvalidateMemory() = 0;
protected:
    TSize m_size;
    TSize m_offset;
    TSize m_alignmentOffset;

    void InitializeMemory(const MemoryInitializeInfo<TSize>& initializeInfo) {
        m_size = initializeInfo.m_size;
        m_offset = initializeInfo.m_offset;
        m_alignmentOffset = initializeInfo.m_alignmentOffset;
    }
};