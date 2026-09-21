#pragma once

#include "IEngineLoop.h"

class VulkanRHLLoop : public IEngineLoop {
public:
    bool Initialize() override;
    bool Tick() override;
    bool Exit() override;
};