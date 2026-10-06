#include "../Public/ModuleSequence.h"

ModuleSequence::ModuleSequence(const std::vector<ModuleBlueprint>& sequenceBlueprint)
: m_sequencePointer(0) {
    ResolveSequenceBlueprint(sequenceBlueprint);
}

ModuleSequence::~ModuleSequence() {
    for (ModuleBase *module: m_sequence) {
        delete module;
    }
}

bool ModuleSequence::HasNextStartUpModule() const {
    return m_sequencePointer < static_cast<int32_t>(m_sequence.size());
}

bool ModuleSequence::HasNextTerminateModule() const {
    return m_sequencePointer >= 0;
}

ModuleBase* ModuleSequence::GetNextStartUpModule() {
    if (!HasNextStartUpModule())
        return nullptr;

    m_sequencePointer = std::max<int32_t>(m_sequencePointer, 0);
    ModuleBase* nextModule = m_sequence[m_sequencePointer];
    ++m_sequencePointer;
    return nextModule;
}

ModuleBase* ModuleSequence::GetNextTerminateModule() {
    if (!HasNextTerminateModule())
        return nullptr;

    m_sequencePointer = std::min<int32_t>(m_sequencePointer, static_cast<int32_t>(m_sequence.size() - 1));
    ModuleBase* nextModule = m_sequence[m_sequencePointer];
    --m_sequencePointer;
    return nextModule;
}

void ModuleSequence::ResolveSequenceBlueprint(const std::vector<ModuleBlueprint>& sequenceBlueprint) {
    m_sequence.reserve(sequenceBlueprint.size());
    for (const ModuleBlueprint& moduleBlueprint: sequenceBlueprint) {
        m_sequence.emplace_back(moduleBlueprint.m_moduleCreateFunc());
        for (std::type_index bindDest: moduleBlueprint.m_moduleBindDestinations) {
            m_moduleAccessMap[bindDest] = m_sequence.size() - 1;
        }
    }
}