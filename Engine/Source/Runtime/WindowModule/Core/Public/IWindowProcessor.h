#pragma once

#include <cstdint>

class Window;

class IWindowProcessor {
public:
    virtual ~IWindowProcessor() = default;
    virtual void ProcessWindowCloseRequest() = 0;
    virtual void ProcessWindowResize(uint32_t newWidth, uint32_t newHeight) = 0;
};