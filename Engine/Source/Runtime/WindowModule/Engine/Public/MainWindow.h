#pragma once

#include "Window.h"

class MainWindow : public Window {
protected:
    IWindowProcessor* CreateWindowProcessor() override;
};