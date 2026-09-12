#pragma once

#include "IAllocatorAdopter.h"

class SimpleToLinearAllocatorAdopter : public IAllocatorAdopter {
public:
    void Adopt(IAllocator *dest, IAllocator *src) override;
};
