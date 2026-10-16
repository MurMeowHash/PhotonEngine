#pragma once

#include "CoreGlobals.h"
#include "Queue/VulkanWorkContext.h"
#include "Resources/VulkanResourceTransition.h"

class VulkanRHLCommandWorkContext : public VulkanWorkContext {
public:
    [[nodiscard]] RHLPipeline GetRHLPipeline() const;
    [[nodiscard]] virtual Photon::Result Execute();
    void BeginTransition(const VulkanRHLPendingTransition& pendingTransition);
    void EndTransition(const VulkanRHLPendingTransition& pendingTransition);
    void ExecMonoTransition(const VulkanRHLPendingTransition& pendingTransition);
protected:
    RHLPipeline m_contextPipeline{};

    [[nodiscard]] Photon::Result InitializeContext(RHLPipeline pipeline);
private:
    static VulkanPipelineImageTransitionScope CreatePipelineImageTransitionScope(const VulkanImageTransitionScope& scope,
        uint32_t queueFamily, bool ignoreStageAccess);

    void ProcessTransition(const VulkanRHLPendingTransition& pendingTransition, bool ignoreSrcStageAccess, bool ignoreDestStageAccess);
};