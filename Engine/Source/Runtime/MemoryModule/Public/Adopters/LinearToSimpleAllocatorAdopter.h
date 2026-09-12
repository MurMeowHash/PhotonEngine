#pragma once

#include "IAllocatorAdopter.h"

class LinearToSimpleAllocatorAdopter : public IAllocatorAdopter {
public:
    void Adopt(IAllocator *dest, IAllocator *src) override;
};