#include "../Public/GameWindowProcessor.h"
#include "PlatformInteractor.h"

void GameWindowProcessor::ProcessWindowCloseRequest() {
    Photon::Core::PlatformInteractor::RequestExit(false);
}