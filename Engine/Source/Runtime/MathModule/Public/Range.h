#pragma once

template<typename T>
class Range {
public:
    Range(T begin, T end)
    : m_begin(begin), m_end(end) { }

    [[nodiscard]] T GetSize() const {
        return m_end - m_begin;
    }

    [[nodiscard]] T GetBegin() const {
        return m_begin;
    }

    [[nodiscard]] T GetEnd() const {
        return m_end;
    }

    void ShrinkForward(T shift) {
        m_begin += shift;
    }

    void ShrinkBackward(T shift) {
        m_end -= shift;
    }

    void OverrideBegin(T begin) {
        m_begin = begin;
    }

    void OverrideEnd(T end) {
        m_end = end;
    }

private:
    T m_begin;
    T m_end;
};