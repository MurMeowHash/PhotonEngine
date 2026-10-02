#pragma once
#include "ModuleBase.h"

class DebugModule : public ModuleBase {
public:
    [[nodiscard]] Photon::Result StartUp() override;
    void Terminate() override;
};
