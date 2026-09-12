#pragma once

#include "Adopters/IAllocatorAdopter.h"

class IAllocatorProcessor {
public:
    virtual bool TryResolveAdopter(IAllocator* externalAllocator, IAllocatorAdopter*& allocatorAdopter) = 0;
    virtual ~IAllocatorProcessor() = default;
};