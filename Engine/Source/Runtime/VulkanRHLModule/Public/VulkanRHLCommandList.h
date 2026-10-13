#pragma once

#include "VulkanRHLGraphicsWorkContext.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include <unordered_map>

class VulkanRHLViewport;
class VulkanRHLTexture;

struct VulkanRHLCommandListCreateInfo {

};

struct VulkanRHLTextureTransition {
    VulkanRHLTexture* m_rhlTexture;
    RHLPipeline m_srcPipeline;
    RHLPipelineUsage m_srcPipelineUsage;
    RHLPipeline m_destPipeline;
    RHLPipelineUsage m_destPipelineUsage;
};

class VulkanRHLCommandList {
public:
    ~VulkanRHLCommandList();
    static VulkanRHLCommandList* Create([[maybe_unused]] const VulkanRHLCommandListCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    VulkanRHLGraphicsWorkContext* GetGraphicsContext() const;
    void Transition(const VulkanRHLResourceTransitionRequestBatch& transitionRequests) const;
    void Transition(VulkanRHLTextureTransition&& textureTransition) const;
    void BeginRenderingViewport(VulkanRHLViewport* rhlViewport) const;
    void EndRenderingViewport(VulkanRHLViewport* rhlViewport) const;
private:
    VulkanRHLGraphicsWorkContext* m_rhlGraphicsWorkContext = nullptr;
    std::unordered_map<RHLPipeline, VulkanRHLCommandWorkContext*> m_commandWorkContexts;

    void AddCommandWorkContext(VulkanRHLCommandWorkContext* workContext);
    [[nodiscard]] VulkanRHLCommandWorkContext* GetCommandWorkContext(RHLPipeline pipeline) const;
};