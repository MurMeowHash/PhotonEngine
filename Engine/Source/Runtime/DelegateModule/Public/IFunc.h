#pragma once

template<typename TReturn, typename... Args>
class IFunc {
public:
    virtual TReturn Invoke(Args... args) const = 0;
    virtual TReturn operator()(Args... args) const = 0;
    virtual ~IFunc() = default;
};