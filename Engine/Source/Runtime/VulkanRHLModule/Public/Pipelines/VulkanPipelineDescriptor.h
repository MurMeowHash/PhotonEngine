#pragma once
#include <correg.h>

class VulkanPipelineDescriptor {
public:
    virtual ~VulkanPipelineDescriptor() = default;
    [[nodiscard]] virtual size_t GetIdentifier() const = 0;
};