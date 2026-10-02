#pragma once

#include "IEngineLoop.h"

class ModuleSequence;
class MainWindow;

class CoreLoop : public IEngineLoop {
public:
    bool Initialize() override;
    bool Tick() override;
    bool Exit() override;

private:
    MainWindow* m_mainWindow = nullptr;

    [[nodiscard]] bool CreateMainWindow();
};