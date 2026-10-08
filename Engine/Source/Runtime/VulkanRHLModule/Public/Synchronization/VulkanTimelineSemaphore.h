#pragma once

#include "VulkanSemaphore.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

class VulkanDevice;

struct VulkanTimelineSemaphoreCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint64_t m_initialValue;
};

class VulkanTimelineSemaphore : public VulkanSemaphore {
public:
    [[nodiscard]] VulkanSemaphoreLockResult ScheduleAcquire() override;
    [[nodiscard]] VulkanSemaphoreLockResult ScheduleRelease() override;
public:
    static VulkanTimelineSemaphore* Create(const VulkanTimelineSemaphoreCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    [[nodiscard]] uint64_t GetCurrentScheduledValue() const;
    [[nodiscard]] Photon::Result TryGetCurrentTimelineValue(uint64_t& value) const;
private:
    VulkanDevice* m_vulkanDevice = nullptr;
    uint64_t m_timelineValue = 0;
};