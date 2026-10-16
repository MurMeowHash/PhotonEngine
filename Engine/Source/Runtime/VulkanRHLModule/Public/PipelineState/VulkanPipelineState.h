#pragma once

#include "VulkanPipelineStateBlueprint.h"
#include "VulkanPipelineStateProperty.h"
#include "VulkanRHLCommandWorkContext.h"
#include "ObjectDelegate.h"

class VulkanCommandBuffer;

template<std::derived_from<VulkanPipelineStateBlueprint> TPipelineBlueprint>
class VulkanPipelineState {
public:
    virtual ~VulkanPipelineState() {
        InvalidateLastCommitBuffer();
    }

    void Commit(VulkanRHLCommandWorkContext* workContext) {
        VulkanCommandBuffer* commitBuffer = workContext->GetCommandBuffer();
        if (commitBuffer != m_lastCommitBuffer) {
            InvalidateLastCommitBuffer();
            commitBuffer->m_onReset += &m_onCommandBufferResetDelegate;
            m_lastCommitBuffer = commitBuffer;
            InvalidatePropertiesDirtyFlag();
        }

        CommitInternal(commitBuffer);
    }

    void Invalidate() {
        InvalidateLastCommitBuffer();
    }

    void SetPipeline(VulkanPipeline* pipeline) {
        m_pipeline.SetProperty(pipeline);
    }

protected:
    TPipelineBlueprint m_pipelineBlueprint;

    VulkanPipelineStateProperty m_pipeline;
    VulkanCommandBuffer* m_lastCommitBuffer = nullptr;

    virtual void CommitInternal(VulkanCommandBuffer* commandBuffer) = 0;
    virtual void InvalidatePropertiesDirtyFlag() = 0;

private:
    ObjectDelegate<VulkanPipelineState> m_onCommandBufferResetDelegate{ this, &VulkanPipelineState::OnCommandBufferReset };

    void OnCommandBufferReset() {
        InvalidatePropertiesDirtyFlag();
    }

    void InvalidateLastCommitBuffer() {
        if (m_lastCommitBuffer != nullptr)
            m_lastCommitBuffer->m_onReset-= &m_onCommandBufferResetDelegate;

        m_lastCommitBuffer = nullptr;
    }
};