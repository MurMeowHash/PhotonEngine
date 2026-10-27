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
#include "VertexFactory/VulkanRHLVertexLayout.h"
#include "VertexFactory/VulkanRHLVertexInput.h"
#include "Resources/VulkanRHLBuffer.h"

struct VulkanDynamicRHLCreateInfo {

};

struct VulkanRHLVertexInputCreateInfo {
    VulkanRHLVertexLayoutDescriptor m_vertexLayoutDescriptor;
    std::vector<VulkanRHLVertexStream> m_vertexStreams;
};

class VulkanRHLFactory;

class VulkanDynamicRHL {
    friend class VulkanRHLFactory;
public:
    ~VulkanDynamicRHL();
    [[nodiscard]] VulkanRHLViewport* CreateRHLViewport(const RHLViewportCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) const;
    [[nodiscard]] VulkanRHLBuffer* CreateRHLBuffer(const RHLBufferCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) const;
    void Tick() const;
    [[nodiscard]] VulkanDevice* GetVulkanDevice() const;
    [[nodiscard]] VulkanInstance* GetVulkanInstance() const;
    [[nodiscard]] VulkanRHLPendingTransition CreateTransition(const VulkanRHLResourceTransitionRequestBatch& transitionRequests) const;
    [[nodiscard]] VulkanRHLVertexInput* CreateVertexInput(const VulkanRHLVertexInputCreateInfo& createInfo) const;
private:
    VulkanInstance* m_vulkanInstance = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanRHLCommandBuffer* m_immediateRHlCommandBuffer = nullptr;
    std::vector<std::unique_ptr<IVulkanRHLSystem>> m_rhlSystems;
    VulkanRHLVertexLayoutCache* m_vertexLayoutCache = nullptr;

    [[nodiscard]] Photon::Result Create([[maybe_unused]] const VulkanDynamicRHLCreateInfo& createInfo);

    [[nodiscard]] Photon::Result CreateVulkanInstance();
    [[nodiscard]] Photon::Result CreateVulkanDevice();

    void SetUpRHLSystems();

    template<std::derived_from<IVulkanRHLSystem> TSystem>
    void AddRHLSystem() {
        m_rhlSystems.emplace_back(std::make_unique<TSystem>());
    }
};
