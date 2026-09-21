#pragma once
#include "IRenderer.h"
#include "RendererGlobals.h"

namespace Photon::RendererFactory {
    IRenderer* CreateRenderer(RendererType rendererType);
};