#pragma once

#include "IEngineFactory.h"

class GameEngineFactory : public IEngineFactory {
public:
    [[nodiscard]] IEngine* CreateEngine() override;
};