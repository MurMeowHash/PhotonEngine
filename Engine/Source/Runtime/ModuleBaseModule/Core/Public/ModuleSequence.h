#pragma once

#include <vector>
#include "ModuleBase.h"
#include <cstdint>
#include "ModuleBlueprint.h"
#include <unordered_map>
#include <typeindex>

class ModuleSequence {
public:
    explicit ModuleSequence(const std::vector<ModuleBlueprint>& sequenceBlueprint);
    ~ModuleSequence();

    [[nodiscard]] bool HasNextStartUpModule() const;
    [[nodiscard]] bool HasNextTerminateModule() const;

    ModuleBase* GetNextStartUpModule();
    ModuleBase* GetNextTerminateModule();

    template<typename TModule>
    bool TryResolveModule(TModule*& module) {
        auto moduleAccessIterator = m_moduleAccessMap.find(std::type_index(typeid(TModule)));
        if (moduleAccessIterator == m_moduleAccessMap.end() || (moduleAccessIterator->second < 0 || moduleAccessIterator->second >= m_sequence.size()))
            return false;

        module = static_cast<TModule*>(m_sequence[moduleAccessIterator->second]);
        return true;
    }

private:
    std::vector<ModuleBase*> m_sequence;
    int32_t m_sequencePointer;
    std::unordered_map<std::type_index, size_t> m_moduleAccessMap;

    void ResolveSequenceBlueprint(const std::vector<ModuleBlueprint>& sequenceBlueprint);
};
