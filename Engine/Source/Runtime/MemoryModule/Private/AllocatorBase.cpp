#include "AllocatorBase.h"
#include "AllocatorProcessor.h"

AllocatorBase::AllocatorBase(std::type_index overrideTypeIndex) {
    m_allocatorProcessor = new AllocatorProcessor(overrideTypeIndex);
}

bool AllocatorBase::TryAdopt(IAllocator *externalAllocator) {
    IAllocatorAdopter* allocatorAdopter;
    if(!m_allocatorProcessor->TryResolveAdopter(externalAllocator, allocatorAdopter))
        return false;

    allocatorAdopter->Adopt(this, externalAllocator);
    return true;
}

AllocatorBase::~AllocatorBase() {
    delete m_allocatorProcessor;
}