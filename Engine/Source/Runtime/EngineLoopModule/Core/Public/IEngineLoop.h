#pragma once

class IEngineLoop {
public:
    virtual bool Initialize() = 0;
    virtual bool Tick() = 0;
    virtual bool Exit() = 0;
    virtual ~IEngineLoop() = default;
};