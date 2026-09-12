#pragma once

#include "IDelegate.h"
#include "ObjectFunc.h"
#include <utility>

template<typename TObject, typename... Args>
class ObjectDelegate : private ObjectFunc<TObject, void, Args...>, public IDelegate<Args...> {
public:
    using Internal = ObjectFunc<TObject, void, Args...>;

    ObjectDelegate(TObject *object, Internal::MemberFunctionHandler memberFunctionHandler)
    : ObjectFunc<TObject, void, Args...>(object, memberFunctionHandler) {}

    void Invoke(Args... args) const override {
        Internal::Invoke(std::forward<Args>(args)...);
    }

    void operator()(Args... args) const override {
        Internal::operator()(std::forward<Args>(args)...);
    }
};