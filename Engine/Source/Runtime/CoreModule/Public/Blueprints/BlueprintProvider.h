#pragma once

#include <unordered_map>
#include "BlueprintDescriptor.h"
#include "IAllocator.h"
#include "SimpleAllocator.h"

template<typename TBlueprint, typename TDescriptor>
requires std::is_base_of_v<BlueprintDescriptor, TDescriptor>
class BlueprintProvider {
public:
    TBlueprint* PoolBlueprint(const TDescriptor& desc) {
        size_t descIdentifier = desc.GetIdentifier();
        auto blueprintIterator = m_blueprints.find(descIdentifier);
        TBlueprint* candidate = nullptr;
        if (blueprintIterator != m_blueprints.end()) {
            candidate = blueprintIterator->second;
        } else {
            void* blueprintMemory = m_allocator->AllocateMemory(sizeof(TBlueprint));
            TBlueprint* blueprint = CreateBlueprint(desc, blueprintMemory);
            m_blueprints[descIdentifier] = blueprint;
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

    virtual ~BlueprintProvider() {
        if (!m_isNativeAllocator)
            return;

        m_allocator->FreeMemory();
        delete m_allocator;
    }

    virtual TBlueprint* CreateBlueprint(const TDescriptor& desc, void* allocatedMemory) = 0;
private:
    std::unordered_map<size_t, TBlueprint*> m_blueprints;
    IAllocator* m_allocator;
    bool m_isNativeAllocator = false;
};
