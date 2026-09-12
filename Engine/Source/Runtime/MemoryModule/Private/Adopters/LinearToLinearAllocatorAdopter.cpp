#include "Adopters/LinearToLinearAllocatorAdopter.h"

void LinearToLinearAllocatorAdopter::Adopt(IAllocator* dest, IAllocator* src) {
    if (dest == src || dest == nullptr || src == nullptr)
        return;

    LinearAllocator* destLinear = static_cast<LinearAllocator*>(dest);
    LinearAllocator* srcLinear  = static_cast<LinearAllocator*>(src);

    if (srcLinear->m_rootChunk == nullptr)
        return;

    if (destLinear->m_rootChunk == nullptr) {
        destLinear->m_rootChunk = srcLinear->m_rootChunk;
        destLinear->m_tailChunk = srcLinear->m_tailChunk;
    } else {
        destLinear->m_tailChunk->m_next = srcLinear->m_rootChunk;
        destLinear->m_tailChunk = srcLinear->m_tailChunk;
    }

    srcLinear->InvalidateAllocator();
}