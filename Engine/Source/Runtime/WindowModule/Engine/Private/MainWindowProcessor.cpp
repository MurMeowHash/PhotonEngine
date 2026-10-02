#include "../Public/MainWindowProcessor.h"
#include "PlatformInteractor.h"

void MainWindowProcessor::ProcessWindowCloseRequest() {
    Photon::Core::PlatformInteractor::RequestExit(false);
}