#include "../Public/DebugModule.h"
#include "Logger.h"
#include <iostream>

Photon::Result DebugModule::StartUp() {
    Photon::Logger::SetOutputStream(&std::cout);
    return Photon::Result::Success;
}

void DebugModule::Terminate() {
    Photon::Logger::SetOutputStream(nullptr);
}