#pragma once
#include "EngineModule.h"

class MockEngineModule : EngineModule {
protected:
    IEngineFactory* CreateEngineFactory() override;
};
