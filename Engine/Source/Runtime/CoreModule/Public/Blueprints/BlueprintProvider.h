#pragma once

#include <concepts>
#include <unordered_map>
#include "BlueprintDescriptor.h"
#include "IAllocator.h"
#include "SimpleAllocator.h"

template<typename TBlueprint, typename TDescriptor>
requires std::is_base_of_v<BlueprintDescriptor, TDescriptor>
    and std::equality_comparable<TDescriptor>
    and std::copy_constructible<TDescriptor>
class BlueprintProvider { // TODO: rename to CacheProvider
public:
    virtual ~BlueprintProvider(){
        for (auto& [descriptor, blueprint] : m_blueprints)
            blueprint->~TBlueprint();

        if (m_isNativeAllocator) {
            m_allocator->FreeMemory();
            delete m_allocator;
        }
    }

    TBlueprint* PoolBlueprint(const TDescriptor& desc) {
        auto blueprintIterator = m_blueprints.find(desc);
        TBlueprint* candidate = nullptr;
        if (blueprintIterator != m_blueprints.end()) {
            candidate = blueprintIterator->second;
        } else {
            void* blueprintMemory = m_allocator->AllocateMemory(sizeof(TBlueprint));
            TBlueprint* blueprint = CreateBlueprint(desc, blueprintMemory);
            m_blueprints.emplace(desc, blueprint);
            candidate = blueprint;
        }

        return candidate;
    }
protected:
    BlueprintProvider(IAllocator* allocator = nullptr) {
        if (allocator == nullptr) {
            allocator = new SimpleAllocator(0);
            m_isNativeAllocator = true;
        }

        m_allocator = allocator;
    }

    virtual TBlueprint* CreateBlueprint(const TDescriptor& desc, void* allocatedMemory) = 0;
private:
    struct DescriptorHasher {
        size_t operator()(const TDescriptor& desc) const {
            return desc.GetIdentifier();
        }
    };

    std::unordered_map<TDescriptor, TBlueprint*, DescriptorHasher> m_blueprints;
    IAllocator* m_allocator;
    bool m_isNativeAllocator = false;
};
