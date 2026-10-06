#pragma once
#include "IEngine.h"

class IEngineFactory {
public:
    virtual ~IEngineFactory() = default;
    [[nodiscard]] virtual IEngine* CreateEngine() = 0;
};
