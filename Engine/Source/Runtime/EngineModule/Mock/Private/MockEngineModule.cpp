#include "../Public/MockEngineModule.h"
#include "MockEngineFactory.h"

IEngineFactory* MockEngineModule::CreateEngineFactory() {
    return new MockEngineFactory();
}
