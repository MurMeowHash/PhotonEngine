#pragma once

#include <functional>
#include <typeindex>
#include "ModuleBase.h"

struct ModuleBlueprint {
    template<typename TModule> requires std::is_base_of_v<ModuleBase, TModule>
    static ModuleBlueprint Create() {
        ModuleBlueprint blueprint;
        blueprint.m_moduleCreateFunc = []() {
            return new TModule();
        };

        blueprint.m_moduleTypeIndex = std::type_index(typeid(TModule));
        return blueprint;
    }

    std::function<ModuleBase*()> m_moduleCreateFunc;
    std::type_index m_moduleTypeIndex = typeid(void);
};