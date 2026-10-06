#pragma once
#include "IEngine.h"

class Window;
class ViewportInteractor;
class Viewport;

class GameEngine : public IEngine {
public:
    [[nodiscard]] Photon::Result Initialize() override;
    [[nodiscard]] Photon::Result Tick() override;
    [[nodiscard]] Photon::Result Exit() override;
    void RenderViewports() override;
private:
    Window* m_gameWindow = nullptr;
    ViewportInteractor* m_gameViewportInteractor = nullptr;
    Viewport* m_gameViewport = nullptr;

    [[nodiscard]] Photon::Result CreateGameWindow();
    [[nodiscard]] Photon::Result CreateGameViewport();
};