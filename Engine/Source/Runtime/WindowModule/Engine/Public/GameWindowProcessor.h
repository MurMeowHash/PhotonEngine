#pragma once
#include "GenericWindowProcessor.h"

class GameWindowProcessor : public GenericWindowProcessor {
public:
    void ProcessWindowCloseRequest() override;
};
