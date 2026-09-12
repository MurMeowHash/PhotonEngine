#pragma once

#include <unordered_set>
#include "IDelegate.h"

template<typename...Args>
class Event {
public:
    virtual void Subscribe(IDelegate<Args...>* callback) {
        m_subscribers.emplace(callback);
    }

    virtual void UnSubscribe(IDelegate<Args...>* callback) {
        m_subscribers.erase(callback);
    }

    virtual void Invoke(Args... args) {
        for (const auto& subscriber : m_subscribers) {
            subscriber->Invoke(std::forward<Args>(args)...);
        }
    }

    void operator+=(IDelegate<Args...>* callback) {
        Subscribe(callback);
    }

    void operator-=(IDelegate<Args...>* callback) {
        UnSubscribe(callback);
    }

    virtual ~Event() = default;

private:
    std::unordered_set<IDelegate<Args...>*> m_subscribers;
};