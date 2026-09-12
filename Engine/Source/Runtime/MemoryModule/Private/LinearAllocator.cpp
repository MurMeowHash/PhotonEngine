#include "../Public/LinearAllocator.h"
#include <cassert>
#include "MemoryOperationsCommon.h"

LinearAllocator::LinearAllocator(size_t uniformChunkSize)
: AllocatorBase(typeid(LinearAllocator)), m_uniformChunkSize(uniformChunkSize) { }

void *LinearAllocator::AllocateMemory(size_t memorySize) {
    assert(memorySize <= m_uniformChunkSize);

    if (m_rootChunk == nullptr) {
        m_rootChunk = new MemoryChunk(m_uniformChunkSize);
        m_tailChunk = m_rootChunk;
    }

    size_t alignment = alignof(std::max_align_t);
    size_t alignedOffset = Rat::MemoryOperationsCommon::AlignForward(m_tailChunk->m_size, alignment);

    if(alignedOffset + memorySize > m_tailChunk->m_capacity) {
        m_tailChunk->m_next = new MemoryChunk(m_uniformChunkSize);
        m_tailChunk = m_tailChunk->m_next;

        alignedOffset = Rat::MemoryOperationsCommon::AlignForward<size_t>(0, alignment);
    }

    if(!m_tailChunk->IsChunkValid())
        return nullptr;

    void* allocatedMemory = m_tailChunk->m_memory + alignedOffset;
    m_tailChunk->m_size = alignedOffset + memorySize;
    return allocatedMemory;
}

void LinearAllocator::FreeMemory() {
    MemoryChunk* currentChunk = m_rootChunk;
    while(currentChunk != nullptr) {
        currentChunk->FreeChunk();
        MemoryChunk* inspectedChunk = currentChunk;
        currentChunk = currentChunk->m_next;
        delete inspectedChunk;
    }

    InvalidateAllocator();
}

void LinearAllocator::InvalidateAllocator() {
    m_rootChunk = nullptr;
    m_tailChunk = nullptr;
}

size_t LinearAllocator::GetAllocatedMemorySize() const {
    size_t totalMemorySize = 0;

    MemoryChunk* currentChunk = m_rootChunk;
    while (currentChunk != nullptr) {
        totalMemorySize += currentChunk->m_size;
        currentChunk = currentChunk->m_next;
    }

    return totalMemorySize;
}