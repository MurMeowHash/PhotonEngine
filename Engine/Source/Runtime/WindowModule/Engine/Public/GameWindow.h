#pragma once

#include "Window.h"

class GameWindow : public Window {
protected:
    [[nodiscard]] Photon::Result PostInitialize() override;
    IWindowProcessor* CreateWindowProcessor() override;
};