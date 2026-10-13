#pragma once
#include "VulkanRHLSystemBase.h"

class VulkanRHLInterruptProcessSystem : public VulkanRHLSystemBase {
public:
    void Tick() override;
    void Dispose() override;
};
