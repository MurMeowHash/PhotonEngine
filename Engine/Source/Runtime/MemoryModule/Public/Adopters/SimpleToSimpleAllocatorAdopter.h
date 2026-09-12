#pragma once

#include "IAllocatorAdopter.h"

class SimpleToSimpleAllocatorAdopter : public IAllocatorAdopter {
public:
    void Adopt(IAllocator *dest, IAllocator *src) override;
};