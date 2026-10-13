#pragma once
#include "IVulkanRHLSystem.h"

class VulkanRHLSystemBase : public IVulkanRHLSystem {
public:
    void Initialize(VulkanDynamicRHL *dynamicRHL) override;

protected:
    VulkanDynamicRHL* m_dynamicRHL = nullptr;
};
