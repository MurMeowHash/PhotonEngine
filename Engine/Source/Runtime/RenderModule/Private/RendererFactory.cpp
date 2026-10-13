#include "../Public/RendererFactory.h"
#include <stdexcept>
#include "CoreUtils.h"
#include "ForwardRenderer.h"

std::unique_ptr<IRenderer> Photon::RendererFactory::CreateRenderer(RendererType rendererType) {
    switch (rendererType) {
        case RendererType::Forward:
            return std::make_unique<ForwardRenderer>();
        default:
            throw std::runtime_error(StringFormatter("Unknown renderer type", static_cast<int>(rendererType)));
    }
}
