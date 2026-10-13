#pragma once
#include "IRenderer.h"
#include "RendererGlobals.h"
#include <memory>

namespace Photon::RendererFactory {
    std::unique_ptr<IRenderer> CreateRenderer(RendererType rendererType);
};