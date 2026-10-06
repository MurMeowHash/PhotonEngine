#include "../Public/EngineModule.h"

Photon::Result EngineModule::StartUp() {
    m_engineFactory = CreateEngineFactory();
    return Photon::Result::Success;
}

void EngineModule::Terminate() {
    delete m_engineFactory;
}

IEngineFactory * EngineModule::GetEngineFactory() const {
    return m_engineFactory;
}