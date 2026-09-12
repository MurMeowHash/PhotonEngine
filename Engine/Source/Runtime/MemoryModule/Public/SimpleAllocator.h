#pragma once

#include "AllocatorBase.h"
#include <cstdint>
#include <vector>

typedef uint8_t byte;

struct MemoryCell {
    byte* m_memory;
    size_t m_memorySize;
};

class SimpleToSimpleAllocatorAdopter;
class LinearToSimpleAllocatorAdopter;
class SimpleToLinearAllocatorAdopter;

class SimpleAllocator : public AllocatorBase {
public:
    explicit SimpleAllocator(size_t allocateCapacity);
    void* AllocateMemory(size_t memorySize) override;
    void FreeMemory() override;
    void InvalidateAllocator() override;
    [[nodiscard]] size_t GetAllocatedMemorySize() const override;
private:
    std::vector<MemoryCell> m_allocatedMemoryCells;

    friend class SimpleToSimpleAllocatorAdopter;
    friend class LinearToSimpleAllocatorAdopter;
    friend class SimpleToLinearAllocatorAdopter;
};