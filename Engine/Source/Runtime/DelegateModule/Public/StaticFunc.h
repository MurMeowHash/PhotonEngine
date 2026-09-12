#pragma once

#include "IFunc.h"
#include <functional>

template<typename TReturn, typename... Args>
class StaticFunc : public IFunc<TReturn, Args...> {
public:
    using FunctionHandler = std::function<TReturn(Args...)>;

    explicit StaticFunc(const FunctionHandler &functionHandler)
            : m_Func(functionHandler) {}

    TReturn Invoke(Args... args) const override {
        return m_Func(std::forward<Args>(args)...);
    }

    TReturn operator()(Args... args) const override {
        return Invoke(std::forward<Args>(args)...);
    }
private:
    FunctionHandler m_Func;
};