#include "../Public/SimpleAllocator.h"
#include <cstdlib>

SimpleAllocator::SimpleAllocator(size_t allocateCapacity)
: AllocatorBase(typeid(SimpleAllocator)) {
    m_allocatedMemoryCells.reserve(allocateCapacity);
}

void* SimpleAllocator::AllocateMemory(size_t memorySize) {
    byte* allocatedMemory;
    if (m_currentCellPointer < m_allocatedMemoryCells.size())
        allocatedMemory = m_allocatedMemoryCells[m_currentCellPointer].m_memory;
    else {
        allocatedMemory = static_cast<byte*>(std::malloc(memorySize));
        m_allocatedMemoryCells.emplace_back(allocatedMemory, memorySize);
    }

    ++m_currentCellPointer;
    return allocatedMemory;
}
void SimpleAllocator::FreeMemory() {
    for(const MemoryCell& memoryCell : m_allocatedMemoryCells) {
        std::free(memoryCell.m_memory);
    }

    ClearMemoryCells();
}

void SimpleAllocator::InvalidateAllocator() {
    m_currentCellPointer = 0;
}

size_t SimpleAllocator::GetAllocatedMemorySize() const {
    size_t totalMemorySize = 0;
    for(const MemoryCell& memoryCell : m_allocatedMemoryCells) {
        totalMemorySize += memoryCell.m_memorySize;
    }

    return totalMemorySize;
}

void SimpleAllocator::ClearMemoryCells() {
    m_allocatedMemoryCells.clear();
    InvalidateAllocator();
}
