#pragma once
#include "IEngineFactory.h"

class MockEngineFactory : public IEngineFactory {
public:
    [[nodiscard]] IEngine* CreateEngine() override;
};
