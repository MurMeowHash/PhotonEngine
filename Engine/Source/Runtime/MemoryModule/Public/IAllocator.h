#pragma once

#include <cstddef>

class IAllocator {
public:
    [[nodiscard]] virtual void* AllocateMemory(size_t memorySize) = 0;
    [[nodiscard]] virtual bool TryAdopt(IAllocator* externalAllocator) = 0;
    virtual void FreeMemory() = 0;
    virtual void InvalidateAllocator() = 0;
    [[nodiscard]] virtual size_t GetAllocatedMemorySize() const = 0;
    virtual ~IAllocator() = default;
};