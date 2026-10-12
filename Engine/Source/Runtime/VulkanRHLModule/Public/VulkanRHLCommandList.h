#pragma once

#include "VulkanRHLGraphicsWorkContext.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

struct VulkanRHLCommandListCreateInfo {

};

struct VulkanRHLTextureTransition {
    VulkanRHLTexture* m_rhlTexture;
    RHLWorkType m_srcWorkType;
    RHLWorkType m_destWorkType;
};

class VulkanRHLCommandList {
public:
    ~VulkanRHLCommandList();
    static VulkanRHLCommandList* Create([[maybe_unused]] const VulkanRHLCommandListCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    VulkanRHLGraphicsWorkContext* GetGraphicsContext() const;
    void Submit() const;
    void Transition(const VulkanRHLResourceTransitionRequestBatch& transitionRequests) const;
    void Transition(VulkanRHLTextureTransition&& textureTransition) const;
private:
    enum { WorkTypeCount = static_cast<uint32_t>(RHLWorkType::Graphics) + 1 };

    VulkanRHLGraphicsWorkContext* m_rhlGraphicsWorkContext = nullptr;
    VulkanRHLCommandWorkContext* m_workContexts[WorkTypeCount]{};

    void AddGeneralWorkContext(VulkanRHLCommandWorkContext* workContext);
    VulkanRHLCommandWorkContext* GetWorkContext(RHLWorkType rhlWork) const;
};