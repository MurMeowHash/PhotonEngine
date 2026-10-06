#include "../Public/GameEngineFactory.h"
#include "GameEngine.h"

IEngine* GameEngineFactory::CreateEngine() {
    return new GameEngine();
}