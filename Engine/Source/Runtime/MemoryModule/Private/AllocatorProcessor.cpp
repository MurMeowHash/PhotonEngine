#include "../Public/AllocatorProcessor.h"
#include "LinearAllocator.h"
#include "SimpleAllocator.h"
#include "Adopters/LinearToLinearAllocatorAdopter.h"
#include "Adopters/SimpleToSimpleAllocatorAdopter.h"
#include "Adopters/SimpleToLinearAllocatorAdopter.h"
#include "Adopters/LinearToSimpleAllocatorAdopter.h"

AllocatorProcessor::AllocatorProcessor(std::type_index ownerTypeIndex)
: m_ownerTypeIndex(ownerTypeIndex) {
    FillCachedAllocatorAdopters();
}

bool AllocatorProcessor::TryResolveAdopter(IAllocator* externalAllocator, IAllocatorAdopter*& allocatorAdopter) {
    auto cachedAllocatorTypeIterator = m_cachedAllocatorTypes.find(externalAllocator);
    if(cachedAllocatorTypeIterator == m_cachedAllocatorTypes.end()) {
        auto allocatorTypeEmplaceResult = m_cachedAllocatorTypes.emplace(externalAllocator, typeid(*externalAllocator));
        cachedAllocatorTypeIterator = allocatorTypeEmplaceResult.first;
    }

    auto allocatorAdopterIterator = m_cachedAllocatorAdopters.find(AllocatorAdopterKey(m_ownerTypeIndex, cachedAllocatorTypeIterator->second));
    if(allocatorAdopterIterator == m_cachedAllocatorAdopters.end())
        return false;

    allocatorAdopter = allocatorAdopterIterator->second;
    return true;
}

void AllocatorProcessor::FillCachedAllocatorAdopters() {
    m_cachedAllocatorAdopters.clear();
    m_cachedAllocatorAdopters[AllocatorAdopterKey(typeid(LinearAllocator), typeid(LinearAllocator))] = new LinearToLinearAllocatorAdopter;
    m_cachedAllocatorAdopters[AllocatorAdopterKey(typeid(SimpleAllocator), typeid(SimpleAllocator))] = new SimpleToSimpleAllocatorAdopter;
    m_cachedAllocatorAdopters[AllocatorAdopterKey(typeid(LinearAllocator), typeid(SimpleAllocator))] = new SimpleToLinearAllocatorAdopter;
    m_cachedAllocatorAdopters[AllocatorAdopterKey(typeid(SimpleAllocator), typeid(LinearAllocator))] = new LinearToSimpleAllocatorAdopter;
}

AllocatorProcessor::~AllocatorProcessor() {
    for(const std::pair<const AllocatorAdopterKey, IAllocatorAdopter*>& cachedAllocatorAdopter : m_cachedAllocatorAdopters) {
        delete cachedAllocatorAdopter.second;
    }
}
