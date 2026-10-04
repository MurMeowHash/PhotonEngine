#pragma once

class Viewport;

class ViewportProcessor {
public:
    explicit ViewportProcessor(Viewport* viewport);
    void RedrawViewport() const;
private:
    Viewport* m_viewport;
};