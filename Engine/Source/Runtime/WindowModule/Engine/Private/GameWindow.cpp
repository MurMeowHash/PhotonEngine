#include "../Public/GameWindow.h"
#include "GameWindowProcessor.h"

IWindowProcessor* GameWindow::CreateWindowProcessor() {
    return new GameWindowProcessor();
}