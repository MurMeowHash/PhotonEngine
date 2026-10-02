#pragma once

#include "CoreGlobals.h"

class ModuleBase {
public:
    virtual ~ModuleBase() = default;

    [[nodiscard]] virtual Photon::Result StartUp() = 0;
    virtual void Terminate() = 0;
};