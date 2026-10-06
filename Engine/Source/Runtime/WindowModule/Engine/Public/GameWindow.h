#pragma once

#include "Window.h"

class GameWindow : public Window {
protected:
    IWindowProcessor* CreateWindowProcessor() override;
};