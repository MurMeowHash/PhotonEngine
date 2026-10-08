#pragma once

#include "VulkanSemaphore.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

class VulkanDevice;

struct VulkanBinarySemaphoreCreateInfo {
    VulkanDevice* m_vulkanDevice;
};

class VulkanBinarySemaphore : public VulkanSemaphore {
public:
    [[nodiscard]] VulkanSemaphoreLockResult ScheduleAcquire() override;
    [[nodiscard]] VulkanSemaphoreLockResult ScheduleRelease() override;
public:
    static VulkanBinarySemaphore* Create(const VulkanBinarySemaphoreCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
};