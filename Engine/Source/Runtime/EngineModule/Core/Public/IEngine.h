#pragma once

#include "CoreGlobals.h"

class IEngine {
public:
    virtual ~IEngine() = default;
    [[nodiscard]] virtual Photon::Result Initialize() = 0;
    [[nodiscard]] virtual Photon::Result Tick() = 0;
    [[nodiscard]] virtual Photon::Result Exit() = 0;
    virtual void RenderViewports() = 0;
};