#pragma once

#include "VulkanRHLSystemBase.h"

class VulkanDeferredDeletionQueue;

class VulkanRHLDDQProcessSystem : public VulkanRHLSystemBase {
public:
    void Initialize(VulkanDynamicRHL *dynamicRHL) override;
    void Tick() override;
    void Dispose() override;

private:
    VulkanDeferredDeletionQueue* m_ddq{};
};
