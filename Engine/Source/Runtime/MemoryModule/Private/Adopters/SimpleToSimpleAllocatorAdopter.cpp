#include "Adopters/SimpleToSimpleAllocatorAdopter.h"
#include "SimpleAllocator.h"

void SimpleToSimpleAllocatorAdopter::Adopt(IAllocator *dest, IAllocator *src) {
    if(dest == src || dest == nullptr || src == nullptr)
        return;

    SimpleAllocator* destSimple = static_cast<SimpleAllocator*>(dest);
    SimpleAllocator* srcSimple = static_cast<SimpleAllocator*>(src);

    destSimple->m_allocatedMemoryCells.insert(destSimple->m_allocatedMemoryCells.end(),
                                              srcSimple->m_allocatedMemoryCells.begin(),
                                              srcSimple->m_allocatedMemoryCells.end());
}
