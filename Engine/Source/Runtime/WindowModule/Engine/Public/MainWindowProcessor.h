#pragma once
#include "IWindowProcessor.h"

class MainWindowProcessor : public IWindowProcessor {
public:
    void ProcessWindowCloseRequest() override;
};
