#pragma once

#include "IAllocatorProcessor.h"
#include "unordered_map"
#include "IAllocator.h"
#include <typeindex>
#include <functional>

struct AllocatorAdopterKey {
    std::type_index m_first;
    std::type_index m_second;

    AllocatorAdopterKey(const std::type_index& first, const std::type_index& second)
    : m_first(first), m_second(second) {}

    bool operator==(const AllocatorAdopterKey& other) const {
        return m_first == other.m_first && m_second == other.m_second;
    }
};

struct AllocatorAdopterKeyHash {
    std::size_t operator()(const AllocatorAdopterKey& key) const {
        std::size_t h1 = key.m_first.hash_code();
        std::size_t h2 = key.m_second.hash_code();

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

class AllocatorProcessor : public IAllocatorProcessor {
public:
    explicit AllocatorProcessor(std::type_index ownerTypeIndex);
    bool TryResolveAdopter(IAllocator* externalAllocator, IAllocatorAdopter*& allocatorAdopter) override;
    ~AllocatorProcessor() override;

private:
    std::type_index m_ownerTypeIndex;
    std::unordered_map<IAllocator*, std::type_index> m_cachedAllocatorTypes;
    std::unordered_map<AllocatorAdopterKey, IAllocatorAdopter*, AllocatorAdopterKeyHash> m_cachedAllocatorAdopters;

    void FillCachedAllocatorAdopters();
};