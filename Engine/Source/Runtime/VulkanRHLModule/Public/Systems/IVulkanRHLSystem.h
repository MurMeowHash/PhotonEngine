#pragma once

class VulkanDynamicRHL;

class IVulkanRHLSystem {
public:
    virtual ~IVulkanRHLSystem() = default;
    virtual void Initialize(VulkanDynamicRHL* dynamicRHL) = 0;
    virtual void Tick() = 0;
    virtual void Dispose() = 0;
};