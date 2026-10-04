#pragma once
#include "IWindowProcessor.h"

class GenericWindowProcessor : public IWindowProcessor {
public:
    void ProcessWindowCloseRequest() override;
    void ProcessWindowResize(uint32_t newWidth, uint32_t newHeight) override;
};
