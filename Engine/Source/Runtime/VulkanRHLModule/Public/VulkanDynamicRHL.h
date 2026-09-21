#pragma once

#include "Device/VulkanDevice.h"
#include "Instance/VulkanInstance.h"

class VulkanDynamicRHL {
public:
    ~VulkanDynamicRHL();
    [[nodiscard]] bool Initialize();
private:
    VulkanInstance* m_vulkanInstance = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;

    [[nodiscard]] bool CreateVulkanInstance();
    [[nodiscard]] bool CreateVulkanDevice();
};
