#include "../Public/Viewport.h"

uint32_t Viewport::GetWidth() const {
    return m_width;
}

uint32_t Viewport::GetHeight() const {
    return m_height;
}

void Viewport::ChangeSize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
}