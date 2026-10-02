#pragma once
#include "IWindowProcessor.h"

class DefaultWindowProcessor : public IWindowProcessor {
public:
    void ProcessWindowCloseRequest() override;
};
