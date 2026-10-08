#include "Adopters/LinearToSimpleAllocatorAdopter.h"
#include "SimpleAllocator.h"
#include "LinearAllocator.h"

void LinearToSimpleAllocatorAdopter::Adopt(IAllocator *dest, IAllocator *src) {
    if(dest == nullptr || src == nullptr)
        return;

    SimpleAllocator* destSimple = static_cast<SimpleAllocator*>(dest);
    LinearAllocator* srcLinear = static_cast<LinearAllocator*>(src);

    MemoryChunk* currentLinearChunk = srcLinear->m_rootChunk;
    size_t linearChunkPlacePointer = 0;
    bool foundLinearChunkPlacePointer = false;
    while(currentLinearChunk != nullptr) {
        if (!foundLinearChunkPlacePointer) {
            if (currentLinearChunk == srcLinear->m_tailChunk)
                foundLinearChunkPlacePointer = true;
            else
                ++linearChunkPlacePointer;
        }

        destSimple->m_allocatedMemoryCells.emplace_back(MemoryCell(currentLinearChunk->m_memory, currentLinearChunk->m_capacity));
        currentLinearChunk = currentLinearChunk->m_next;
    }

    destSimple->m_currentCellPointer = linearChunkPlacePointer;
    srcLinear->ClearChunkReferences();
}