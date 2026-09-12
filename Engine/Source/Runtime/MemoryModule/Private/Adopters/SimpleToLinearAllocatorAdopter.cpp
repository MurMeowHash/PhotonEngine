#include "Adopters/SimpleToLinearAllocatorAdopter.h"
#include "LinearAllocator.h"
#include "SimpleAllocator.h"

void SimpleToLinearAllocatorAdopter::Adopt(IAllocator *dest, IAllocator *src) {
    if(dest == nullptr || src == nullptr)
        return;

    LinearAllocator* destLinear = static_cast<LinearAllocator*>(dest);
    SimpleAllocator* srcSimple = static_cast<SimpleAllocator*>(src);

    for(const MemoryCell& simpleAllocatorCell : srcSimple->m_allocatedMemoryCells) {
        if(destLinear->m_rootChunk == nullptr) {
            destLinear->m_rootChunk = new MemoryChunk(simpleAllocatorCell.m_memory, simpleAllocatorCell.m_memorySize);
            destLinear->m_tailChunk = destLinear->m_rootChunk;
        } else {
            destLinear->m_tailChunk->m_next = new MemoryChunk(simpleAllocatorCell.m_memory, simpleAllocatorCell.m_memorySize);
            destLinear->m_tailChunk = destLinear->m_tailChunk->m_next;
        }
    }
}