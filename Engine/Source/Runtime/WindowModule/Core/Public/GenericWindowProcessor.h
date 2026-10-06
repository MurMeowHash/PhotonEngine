#pragma once
#include "IWindowProcessor.h"

class GenericWindowProcessor : public IWindowProcessor {
public:
    void SetWindow(Window *window) override;
    void ProcessWindowCloseRequest() override;
    void ProcessWindowResize(uint32_t newWidth, uint32_t newHeight) override;
private:
    Window* m_window = nullptr;
};
