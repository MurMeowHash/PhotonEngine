#pragma once
#include "IRenderer.h"

class ForwardRenderer : public IRenderer {
public:
    void Render() override;
};
