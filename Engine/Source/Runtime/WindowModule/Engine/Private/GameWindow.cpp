#include "../Public/GameWindow.h"
#include "GameWindowProcessor.h"

Photon::Result GameWindow::PostInitialize() {
    Photon::Result initResult = Window::PostInitialize();
    if (initResult != Photon::Result::Success)
        return initResult;

    initResult = m_viewportService->AddViewport(RenderRect(0, static_cast<float>(m_width), 0, static_cast<float>(m_height)));
    return initResult;
}

IWindowProcessor* GameWindow::CreateWindowProcessor() {
    return new GameWindowProcessor();
}