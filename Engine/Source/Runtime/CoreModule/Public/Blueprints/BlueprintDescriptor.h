#pragma once

#include <cstdint>

class BlueprintDescriptor {
public:
    virtual ~BlueprintDescriptor() = default;
    [[nodiscard]] virtual size_t GetIdentifier() const = 0;
};