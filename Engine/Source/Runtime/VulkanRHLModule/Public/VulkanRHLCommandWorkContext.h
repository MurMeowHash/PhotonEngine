#pragma once

#include "CoreGlobals.h"
#include "VulkanGlobals.h"
#include "Queue/VulkanWorkContext.h"
#include "Resources/VulkanResourceTransition.h"

class VulkanRHLCommandWorkContext : public VulkanWorkContext {
public:
    virtual RHLWorkType GetVulkanRHLWorkType() = 0;

    [[nodiscard]] Photon::Result FinalizeWork();
    void BeginTransition(const VulkanRHLPendingTransition& pendingTransition);
    void EndTransition(const VulkanRHLPendingTransition& pendingTransition);
protected:
    [[nodiscard]] Photon::Result InitializeContext();
private:
    static VulkanPipelineImageTransitionScope CreatePipelineImageTransitionScope(const VulkanImageTransitionScope& scope,
        uint32_t queueFamily, bool ignoreStageAccess);

    void ProcessTransition(const VulkanRHLPendingTransition& pendingTransition, bool ignoreSrcStageAccess, bool ignoreDestStageAccess);
};
