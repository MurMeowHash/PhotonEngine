#pragma once

#include "IAllocator.h"

class IAllocatorAdopter {
public:
    virtual void Adopt(IAllocator* dest, IAllocator* src) = 0;
    virtual ~IAllocatorAdopter() = default;
};