#pragma once

#include "IEngineLoop.h"

class CoreLoop : public IEngineLoop {
public:
    bool Initialize() override;
    bool Tick() override;
    bool Exit() override;
};