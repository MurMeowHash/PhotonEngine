#pragma once

class IWindowProcessor {
public:
    virtual ~IWindowProcessor() = default;
    virtual void ProcessWindowCloseRequest() = 0;
};