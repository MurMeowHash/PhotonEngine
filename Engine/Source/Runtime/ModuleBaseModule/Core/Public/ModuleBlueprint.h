#pragma once

#include <functional>
#include <typeindex>
#include "ModuleBase.h"

template<std::derived_from<ModuleBase> TSrcModule>
struct ModuleBlueprintPendingBinding;

struct ModuleBlueprint {
    template<std::derived_from<ModuleBase> TModule>
    static ModuleBlueprintPendingBinding<TModule> Bind() {
        return ModuleBlueprintPendingBinding<TModule>();
    }

    std::function<ModuleBase*()> m_moduleCreateFunc;
    std::vector<std::type_index> m_moduleBindDestinations;
};

template<std::derived_from<ModuleBase> TSrcModule>
struct ModuleBlueprintPendingBinding {
    template<std::derived_from<ModuleBase> TDestModule>
    ModuleBlueprintPendingBinding& To() {
        m_bindingDestinations.emplace_back(std::type_index(typeid(TDestModule)));
        return *this;
    }

    ModuleBlueprint Finalize() {
        ModuleBlueprint blueprint;
        blueprint.m_moduleCreateFunc = []() {
            return new TSrcModule();
        };
        blueprint.m_moduleBindDestinations = m_bindingDestinations;
        return blueprint;
    }
private:
    std::vector<std::type_index> m_bindingDestinations;
};