#include "../Public/GameEngine.h"
#include "GameWindow.h"
#include "Window.h"
#include "ForwardViewport.h"
#include "ViewportInteractor.h"

Photon::Result GameEngine::Initialize() {
    Photon::Result windowCreateResult = CreateGameWindow();
    if (windowCreateResult != Photon::Result::Success)
        return windowCreateResult;

    Photon::Result viewportCreateResult = CreateGameViewport();
    if (viewportCreateResult != Photon::Result::Success)
        return viewportCreateResult;

    return Photon::Result::Success;
}

Photon::Result GameEngine::Tick() {
    RenderViewports();
    return Photon::Result::Success;
}

Photon::Result GameEngine::Exit() {
    delete m_gameViewportInteractor;
    delete m_gameViewport;
    delete m_gameWindow;
    return Photon::Result::Success;
}

void GameEngine::RenderViewports() {
    m_gameViewportInteractor->RenderViewport();
}

Photon::Result GameEngine::CreateGameWindow() {
    WindowCreateInfo createInfo(1920, 1200, false, "Photon Engine"); // TODO: create configuration
    InOutCreateParams<Photon::Result> inOutCreateParams;
    m_gameWindow = Window::Create<GameWindow>(createInfo, &inOutCreateParams);
    return inOutCreateParams.m_result;
}

Photon::Result GameEngine::CreateGameViewport() {
    m_gameViewportInteractor = new ViewportInteractor();
    ForwardViewportCreateInfo viewportCreateInfo{};
    viewportCreateInfo.m_viewportWindowHandle = m_gameWindow->GetHandle();
    viewportCreateInfo.m_viewportWidth = m_gameWindow->GetWidth();
    viewportCreateInfo.m_viewportHeight = m_gameWindow->GetHeight();
    InOutCreateParams<Photon::Result> viewportInOutCreateParams{};
    ForwardViewport* forwardViewport = ForwardViewport::Create(viewportCreateInfo, &viewportInOutCreateParams);
    if (viewportInOutCreateParams.m_result != Photon::Result::Success)
        return viewportInOutCreateParams.m_result;

    m_gameViewport = forwardViewport;
    m_gameViewportInteractor->ConnectViewport(m_gameViewport);
    m_gameWindow->AttachViewport(m_gameViewportInteractor);
    return Photon::Result::Success;
}