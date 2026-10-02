#include "../Public/MainWindow.h"
#include "MainWindowProcessor.h"

IWindowProcessor* MainWindow::CreateWindowProcessor() {
    return new MainWindowProcessor();
}
