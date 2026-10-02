#pragma once

#include <functional>
#include <string_view>
#include "Blueprints/BlueprintDescriptor.h"

class WindowClassDescriptor : public BlueprintDescriptor {
public:
    const char* m_className;

    [[nodiscard]] size_t GetIdentifier() const override {
        return std::hash<std::string_view>{}(m_className);
    }
};