#pragma once

#include "VulkanRHLGraphicsWorkContext.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include <unordered_map>

class VulkanRHLViewport;
class VulkanRHLTexture;

struct VulkanRHLCommandBufferCreateInfo {

};

struct VulkanRHLTextureTransition {
    VulkanRHLTexture* m_rhlTexture;
    RHLPipeline m_srcPipeline;
    RHLPipelineUsage m_srcPipelineUsage;
    RHLPipeline m_destPipeline;
    RHLPipelineUsage m_destPipelineUsage;
};

class VulkanRHLCommandBuffer {
public:
    virtual ~VulkanRHLCommandBuffer();

    template<std::derived_from<VulkanRHLCommandBuffer> TCommandBuffer>
    static TCommandBuffer* Create([[maybe_unused]] const VulkanRHLCommandBufferCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr) {
        InOutCreateParams<Photon::Result> contextInOutCreateParams{};
        VulkanRHLGraphicsWorkContext* graphicsWorkContext = VulkanRHLGraphicsWorkContext::Create({}, &contextInOutCreateParams);
        if (contextInOutCreateParams.m_result != Photon::Result::Success) {
            Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
            return nullptr;
        }

        TCommandBuffer* instance = Photon::AllocateObject<TCommandBuffer>(inOutCreateParams);
        instance->m_rhlGraphicsWorkContext = graphicsWorkContext;
        instance->AddCommandWorkContext(graphicsWorkContext);
        Photon::PushResult(Photon::Result::Success, inOutCreateParams);
        return instance;
    }

    void Reset();

    VulkanRHLGraphicsWorkContext* GetGraphicsContext() const;
    void Transition(const VulkanRHLResourceTransitionRequestBatch& transitionRequests);
    void Transition(VulkanRHLTextureTransition&& textureTransition);

    void SetViewport(const VulkanRHLViewportRect& viewportRect);
    void SetScissor(const VulkanRHLRect& scissorRect);
    void BindPipeline(const VulkanRHLGraphicsPipelineDescriptor& pipelineDesc);
    void DrawPrimitive(uint32_t vertexCount);
    void BeginRenderPass(const VulkanRHLRenderPassBeginInfo& passBeginInfo);
    void EndRenderPass();

    void* BeginWriteBuffer(VulkanRHLBuffer* rhlBuffer, uint64_t offset, uint64_t size);
    void EndWriteBuffer(VulkanRHLBuffer* rhlBuffer);
protected:
    VulkanRHLGraphicsWorkContext* m_rhlGraphicsWorkContext = nullptr;
    std::unordered_map<RHLPipeline, VulkanRHLCommandWorkContext*> m_commandWorkContexts;

private:
    void AddCommandWorkContext(VulkanRHLCommandWorkContext* workContext);
    [[nodiscard]] VulkanRHLCommandWorkContext* GetCommandWorkContext(RHLPipeline pipeline) const;
};