#pragma once

#include "CoreGlobals.h"
#include "Device/VulkanDevice.h"
#include "Instance/VulkanInstance.h"
#include "Viewport/VulkanRHLViewport.h"

struct VulkanDynamicRHLCreateInfo {

};

class VulkanRHLFactory;

class VulkanDynamicRHL {
    friend class VulkanRHLFactory;
public:
    ~VulkanDynamicRHL();
    [[nodiscard]] VulkanRHLViewport* CreateVulkanRHLViewport(const RHLViewportCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) const;
private:
    VulkanInstance* m_vulkanInstance = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;

    [[nodiscard]] Photon::Result Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo& createInfo);

    [[nodiscard]] Photon::Result CreateVulkanInstance();
    [[nodiscard]] Photon::Result CreateVulkanDevice();
};
