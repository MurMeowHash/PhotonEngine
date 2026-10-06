#include "../Public/GameEngineModule.h"
#include "GameEngineFactory.h"

IEngineFactory* GameEngineModule::CreateEngineFactory() {
    return new GameEngineFactory();
}
