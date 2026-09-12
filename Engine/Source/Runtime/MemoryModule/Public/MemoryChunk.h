#pragma once
#include <cstdint>

typedef uint8_t byte;

struct MemoryChunk {
    MemoryChunk(size_t capacity)
    : m_next(nullptr), m_capacity(capacity), m_size(0), m_memory(nullptr) {
        m_memory = static_cast<byte*>(std::malloc(m_capacity));
    }

    MemoryChunk(byte* memory, size_t memorySize)
    : m_next(nullptr), m_capacity(memorySize), m_size(memorySize), m_memory(memory) { }

    [[nodiscard]] bool IsChunkValid() const { return m_memory != nullptr; }

    void FreeChunk() {
        std::free(m_memory);
        m_memory = nullptr;
        m_size = 0;
    }

    MemoryChunk* m_next;
    size_t m_capacity;
    size_t m_size;
    byte* m_memory;
};