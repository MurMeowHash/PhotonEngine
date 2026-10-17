#pragma once

#include "CoreGlobals.h"
#include "Device/VulkanDevice.h"
#include "Instance/VulkanInstance.h"
#include "Viewport/VulkanRHLViewport.h"
#include "VulkanRHLCommandBuffer.h"
#include "VulkanGlobals.h"
#include "Resources/VulkanResourceTransition.h"
#include "Systems/IVulkanRHLSystem.h"
#include <memory>

struct VulkanDynamicRHLCreateInfo {

};

class VulkanRHLFactory;

class VulkanDynamicRHL {
    friend class VulkanRHLFactory;
public:
    ~VulkanDynamicRHL();
    [[nodiscard]] VulkanRHLViewport* CreateVulkanRHLViewport(const RHLViewportCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) const;
    void Tick() const;
    [[nodiscard]] VulkanDevice* GetVulkanDevice() const;
    [[nodiscard]] VulkanInstance* GetVulkanInstance() const;
    [[nodiscard]] VulkanRHLPendingTransition CreateTransition(const VulkanRHLResourceTransitionRequestBatch& transitionRequests) const;
private:
    VulkanInstance* m_vulkanInstance = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanRHLCommandBuffer* m_immediateRHlCommandBuffer = nullptr;
    std::vector<std::unique_ptr<IVulkanRHLSystem>> m_rhlSystems;

    [[nodiscard]] Photon::Result Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo& createInfo);

    [[nodiscard]] Photon::Result CreateVulkanInstance();
    [[nodiscard]] Photon::Result CreateVulkanDevice();

    void SetUpRHLSystems();

    template<std::derived_from<IVulkanRHLSystem> TSystem>
    void AddRHLSystem() {
        m_rhlSystems.emplace_back(std::make_unique<TSystem>());
    }
};
