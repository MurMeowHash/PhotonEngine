#pragma once
#include "IDelegate.h"
#include "StaticFunc.h"
#include <utility>

template<typename... Args>
class StaticDelegate : private StaticFunc<void, Args...>, public IDelegate<Args...> {
public:
    using Internal = StaticFunc<void, Args...>;

    explicit StaticDelegate(const Internal::FunctionHandler &functionHandler)
    : StaticFunc<void, Args...>(functionHandler) {}

    void Invoke(Args... args) const override {
        Internal::Invoke(std::forward<Args>(args)...);
    }

    void operator()(Args... args) const override {
        Internal::operator()(std::forward<Args>(args)...);
    }
};