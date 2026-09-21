#pragma once

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void Render(/*some render data*/) = 0;
};