#include "../Public/VulkanRHLCommandList.h"
#include "VulkanDynamicRHL.h"

using Photon::Vulkan::g_vulkanDynamicRHL;

VulkanRHLCommandList::~VulkanRHLCommandList() {
    delete m_rhlGraphicsWorkContext;
}

VulkanRHLCommandList* VulkanRHLCommandList::Create([[maybe_unused]] const VulkanRHLCommandListCreateInfo &createInfo,
                                                   InOutCreateParams<Photon::Result> *inOutCreateParams) {
    InOutCreateParams<Photon::Result> contextInOutCreateParams{};
    VulkanRHLGraphicsWorkContext* graphicsWorkContext = VulkanRHLGraphicsWorkContext::Create({}, &contextInOutCreateParams);
    if (contextInOutCreateParams.m_result != Photon::Result::Success) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanRHLCommandList* instance = Photon::AllocateObject<VulkanRHLCommandList>(inOutCreateParams);
    instance->m_rhlGraphicsWorkContext = graphicsWorkContext;
    instance->AddCommandWorkContext(graphicsWorkContext);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanRHLGraphicsWorkContext* VulkanRHLCommandList::GetGraphicsContext() const {
    return m_rhlGraphicsWorkContext;
}

void VulkanRHLCommandList::Transition(const VulkanRHLResourceTransitionRequestBatch &transitionRequests) const {
    VulkanRHLPendingTransition pendingTransition = g_vulkanDynamicRHL->CreateTransition(transitionRequests);
    if (pendingTransition.IsCrossPipeline()) {
        GetCommandWorkContext(pendingTransition.GetSrcPipeline())->BeginTransition(pendingTransition);
        GetCommandWorkContext(pendingTransition.GetDestPipeline())->EndTransition(pendingTransition);
    } else {
        GetCommandWorkContext(pendingTransition.GetSrcPipeline())->ExecMonoTransition(pendingTransition);
    }
}

void VulkanRHLCommandList::Transition(VulkanRHLTextureTransition &&textureTransition) const {
    VulkanRHLResourceTransitionRequestBatch transitionBatch{textureTransition.m_srcPipeline, textureTransition.m_destPipeline};
    transitionBatch.AddTextureTransitionRequest({textureTransition.m_rhlTexture, textureTransition.m_srcPipelineUsage, textureTransition.m_destPipelineUsage});
    Transition(transitionBatch);
}

void VulkanRHLCommandList::BeginRenderingViewport(VulkanRHLViewport *rhlViewport) const {
    rhlViewport->AdvanceBackBuffer(m_rhlGraphicsWorkContext);
}

void VulkanRHLCommandList::EndRenderingViewport(VulkanRHLViewport *rhlViewport) const {
    rhlViewport->Present(m_rhlGraphicsWorkContext);
}

void VulkanRHLCommandList::AddCommandWorkContext(VulkanRHLCommandWorkContext *workContext) {
    m_commandWorkContexts[workContext->GetRHLPipeline()] = workContext;
}

VulkanRHLCommandWorkContext* VulkanRHLCommandList::GetCommandWorkContext(RHLPipeline pipeline) const {
    auto workContextIterator = m_commandWorkContexts.find(pipeline);
    if (workContextIterator == m_commandWorkContexts.end())
        return nullptr;

    return workContextIterator->second;
}