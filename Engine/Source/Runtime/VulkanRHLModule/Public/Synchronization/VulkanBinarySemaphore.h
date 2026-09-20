#pragma once

#include "VulkanSemaphore.h"

class VulkanDevice;

struct VulkanBinarySemaphoreCreateInfo {
    VulkanDevice* m_vulkanDevice;
};

class VulkanBinarySemaphore : public VulkanSemaphore {
public:
    [[nodiscard]] bool Create(const VulkanBinarySemaphoreCreateInfo& createInfo);
};