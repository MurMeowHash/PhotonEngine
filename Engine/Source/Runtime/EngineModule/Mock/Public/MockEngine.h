#pragma once
#include "IEngine.h"

class MockEngine : public IEngine {
public:
    [[nodiscard]] Photon::Result Initialize() override;
    [[nodiscard]] Photon::Result Tick() override;
    [[nodiscard]] Photon::Result Exit() override;
    void RenderViewports() override;
};
