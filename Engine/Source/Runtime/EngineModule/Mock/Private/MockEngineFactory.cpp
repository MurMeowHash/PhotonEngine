#include "../Public/MockEngineFactory.h"
#include "MockEngine.h"

IEngine* MockEngineFactory::CreateEngine() {
    return new MockEngine();
}