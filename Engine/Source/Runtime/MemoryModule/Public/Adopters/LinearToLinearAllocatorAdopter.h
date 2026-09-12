#pragma once

#include "IAllocatorAdopter.h"
#include "LinearAllocator.h"

class LinearToLinearAllocatorAdopter : public IAllocatorAdopter {
public:
    void Adopt(IAllocator* dest, IAllocator* src) override;
};