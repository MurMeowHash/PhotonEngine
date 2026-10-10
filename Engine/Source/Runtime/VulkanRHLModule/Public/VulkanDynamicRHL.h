#pragma once

#include "CoreGlobals.h"
#include "Device/VulkanDevice.h"
#include "Instance/VulkanInstance.h"
#include "Viewport/VulkanRHLViewport.h"
#include "VulkanRHLCommandList.h"

struct VulkanDynamicRHLCreateInfo {

};

class VulkanRHLFactory;

class VulkanDynamicRHL {
    friend class VulkanRHLFactory;
public:
    ~VulkanDynamicRHL();
    [[nodiscard]] VulkanRHLViewport* CreateVulkanRHLViewport(const RHLViewportCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) const;
    [[nodiscard]] VulkanDevice* GetVulkanDevice() const;
    [[nodiscard]] VulkanRHLCommandList* GetImmediateRHLCommandList() const;
private:
    VulkanInstance* m_vulkanInstance = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanRHLCommandList* m_immediateRHlCommandList = nullptr;

    [[nodiscard]] Photon::Result Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo& createInfo);

    [[nodiscard]] Photon::Result CreateVulkanInstance();
    [[nodiscard]] Photon::Result CreateVulkanDevice();
};
