#include "../Public/RendererFactory.h"
#include <stdexcept>
#include "CoreUtils.h"
#include "ForwardRenderer.h"

IRenderer* Photon::RendererFactory::CreateRenderer(RendererType rendererType) {
    switch (rendererType) {
        case RendererType::Forward:
            return new ForwardRenderer();
        default:
            throw std::runtime_error(StringFormatter("Unknown renderer type", static_cast<int>(rendererType)));
    }
}
