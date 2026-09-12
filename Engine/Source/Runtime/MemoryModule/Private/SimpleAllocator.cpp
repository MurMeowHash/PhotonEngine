#include "../Public/SimpleAllocator.h"
#include <cstdlib>

SimpleAllocator::SimpleAllocator(size_t allocateCapacity)
: AllocatorBase(typeid(SimpleAllocator)) {
    m_allocatedMemoryCells.reserve(allocateCapacity);
}

void* SimpleAllocator::AllocateMemory(size_t memorySize) {
    byte* allocatedMemory = static_cast<byte*>(std::malloc(memorySize));
    m_allocatedMemoryCells.emplace_back(MemoryCell(allocatedMemory, memorySize));
    return allocatedMemory;
}
void SimpleAllocator::FreeMemory() {
    for(const MemoryCell& memoryCell : m_allocatedMemoryCells) {
        std::free(memoryCell.m_memory);
    }

    InvalidateAllocator();
}

void SimpleAllocator::InvalidateAllocator() {
    m_allocatedMemoryCells.clear();
}

size_t SimpleAllocator::GetAllocatedMemorySize() const {
    size_t totalMemorySize = 0;
    for(const MemoryCell& memoryCell : m_allocatedMemoryCells) {
        totalMemorySize += memoryCell.m_memorySize;
    }

    return totalMemorySize;
}
