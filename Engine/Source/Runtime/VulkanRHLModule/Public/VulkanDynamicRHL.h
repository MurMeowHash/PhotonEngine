#pragma once

#include "CoreGlobals.h"
#include "Device/VulkanDevice.h"
#include "Instance/VulkanInstance.h"
#include "Viewport/VulkanRHLViewport.h"
#include "VulkanRHLCommandList.h"
#include "VulkanGlobals.h"

struct VulkanDynamicRHLCreateInfo {

};

class VulkanRHLFactory;

using Photon::Vulkan::RHLPipelineUsage;

class VulkanDynamicRHL {
    friend class VulkanRHLFactory;
public:
    ~VulkanDynamicRHL();
    [[nodiscard]] VulkanRHLViewport* CreateVulkanRHLViewport(const RHLViewportCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) const;
    [[nodiscard]] VulkanDevice* GetVulkanDevice() const;
    [[nodiscard]] VulkanRHLCommandList* GetImmediateRHLCommandList() const;
    void TransitionTexture(VulkanRHLTexture* rhlTexture, RHLPipelineUsage srcUsage, RHLPipelineUsage dstUsage) const;
private:
    VulkanInstance* m_vulkanInstance = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanRHLCommandList* m_immediateRHlCommandList = nullptr;

    [[nodiscard]] Photon::Result Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo& createInfo);

    [[nodiscard]] Photon::Result CreateVulkanInstance();
    [[nodiscard]] Photon::Result CreateVulkanDevice();

    [[nodiscard]] ImageTransitionScope ResolveImageTransitionScopeFromUsage(RHLPipelineUsage usage, uint32_t queueFamilyIndex) const;
    void ResolveTransitionQueueFamilyIndices(RHLPipelineUsage srcUsage, RHLPipelineUsage dstUsage,
        uint32_t& srcQueueFamilyIndex, uint32_t& dstQueueFamilyIndex) const;
    uint32_t GetQueueFamilyFromPipeline(RHLPipelineUsage pipelineUsage) const;
};
