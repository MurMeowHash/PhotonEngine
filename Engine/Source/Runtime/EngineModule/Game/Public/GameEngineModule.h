#pragma once
#include "EngineModule.h"

class GameEngineModule : public EngineModule {
protected:
    IEngineFactory* CreateEngineFactory() override;
};
