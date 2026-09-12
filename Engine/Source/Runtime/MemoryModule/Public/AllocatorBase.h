#pragma once

#include "IAllocator.h"
#include <typeindex>
#include "IAllocatorProcessor.h"

class AllocatorBase : public IAllocator {
public:
    explicit AllocatorBase(std::type_index overrideTypeIndex);
    bool TryAdopt(IAllocator *externalAllocator) override;
    ~AllocatorBase() override;
protected:
    IAllocatorProcessor* m_allocatorProcessor;
};