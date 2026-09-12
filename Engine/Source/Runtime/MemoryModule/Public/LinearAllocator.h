#pragma once

#include "AllocatorBase.h"
#include <cstdlib>
#include "MemoryChunk.h"

class LinearToLinearAllocatorAdopter;
class LinearToSimpleAllocatorAdopter;
class SimpleToLinearAllocatorAdopter;

class LinearAllocator : public AllocatorBase {
public:
    explicit LinearAllocator(size_t uniformChunkSize);

    void* AllocateMemory(size_t memorySize) override;
    void FreeMemory() override;
    void InvalidateAllocator() override;
    [[nodiscard]] size_t GetAllocatedMemorySize() const override;

private:
    MemoryChunk* m_rootChunk = nullptr;
    MemoryChunk* m_tailChunk = nullptr;

    size_t m_uniformChunkSize;

    friend class LinearToLinearAllocatorAdopter;
    friend class LinearToSimpleAllocatorAdopter;
    friend class SimpleToLinearAllocatorAdopter;
};