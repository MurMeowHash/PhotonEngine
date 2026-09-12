#pragma once

#include "IFunc.h"
#include <utility>

template<typename TObject, typename TReturn, typename... Args>
class ObjectFunc : public IFunc<TReturn, Args...> {
public:
    using MemberFunctionHandler = TReturn(TObject::*)(Args...);

    ObjectFunc(TObject *object, MemberFunctionHandler memberFunctionHandler)
    : m_object(object), m_MemberFunc(memberFunctionHandler) {}

    TReturn Invoke(Args... args) const override {
        return (m_object->*m_MemberFunc)(std::forward<Args>(args)...);
    }

    TReturn operator()(Args... args) const override {
        return Invoke(std::forward<Args>(args)...);
    }

private:
    TObject *m_object;
    MemberFunctionHandler m_MemberFunc;
};