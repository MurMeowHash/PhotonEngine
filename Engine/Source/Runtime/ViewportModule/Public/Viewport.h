#pragma once
#include "RenderData/RenderTarget.h"
#include <cstdint>

class Viewport : public RenderTarget {
public:
    [[nodiscard]] uint32_t GetWidth() const;
    [[nodiscard]] uint32_t GetHeight() const;

    void ChangeSize(uint32_t width, uint32_t height);
private:
    uint32_t m_width{};
    uint32_t m_height{};
};