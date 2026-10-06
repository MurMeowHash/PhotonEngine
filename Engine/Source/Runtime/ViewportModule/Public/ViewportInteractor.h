#pragma once

#include <cstdint>

class Viewport;

class ViewportInteractor {
public:
    void ConnectViewport(Viewport* viewport);
    void RenderViewport() const;
    void ChangeViewportSize(uint32_t windowWidth, uint32_t windowHeight);
private:
    Viewport* m_viewport = nullptr;
};