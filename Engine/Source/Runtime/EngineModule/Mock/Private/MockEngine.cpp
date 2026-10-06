#include "../Public/MockEngine.h"

Photon::Result MockEngine::Initialize() {
    return Photon::Result::UnknownFailure;
}

Photon::Result MockEngine::Tick() {
    return Photon::Result::UnknownFailure;
}

Photon::Result MockEngine::Exit() {
    return Photon::Result::UnknownFailure;
}

void MockEngine::RenderViewports() {}