#pragma once

#include "VulkanSemaphore.h"

class VulkanDevice;

struct VulkanTimelineSemaphoreCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint64_t m_initialValue;
};

class VulkanTimelineSemaphore : public VulkanSemaphore {
public:
    [[nodiscard]] bool Create(const VulkanTimelineSemaphoreCreateInfo& createInfo);
    bool WaitForValue(uint64_t waitValue, uint64_t timeout = UINT64_MAX) const;
    bool SignalValue(uint64_t signalValue) const;
    [[nodiscard]] bool TryGetCurrentValue(uint64_t& value) const;
private:
    VulkanDevice* m_vulkanDevice = nullptr;
};