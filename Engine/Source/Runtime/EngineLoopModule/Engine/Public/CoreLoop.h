#pragma once

#include "IEngineLoop.h"

class ModuleSequence;
class GameWindow;

class CoreLoop : public IEngineLoop {
public:
    bool Initialize() override;
    bool Tick() override;
    bool Exit() override;

private:
    GameWindow* m_gameWindow = nullptr;

    [[nodiscard]] bool CreateGameWindow();
};