#pragma once
#include "IEngineFactory.h"
#include "ModuleBase.h"

class EngineModule : public ModuleBase {
public:
    [[nodiscard]] Photon::Result StartUp() override;
    void Terminate() override;

    IEngineFactory* GetEngineFactory() const;
protected:
    virtual IEngineFactory* CreateEngineFactory() = 0;
private:
    IEngineFactory* m_engineFactory = nullptr;
};
