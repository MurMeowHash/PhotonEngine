#pragma once

#include "IEngineLoop.h"

class ModuleSequence;
class GameWindow;
class IEngine;

class CoreLoop : public IEngineLoop {
public:
    bool Initialize() override;
    bool Tick() override;
    bool Exit() override;

private:
    IEngine* m_engine = nullptr;
};