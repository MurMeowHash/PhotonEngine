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
    instance->AddGeneralWorkContext(graphicsWorkContext);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanRHLGraphicsWorkContext* VulkanRHLCommandList::GetGraphicsContext() const {
    return m_rhlGraphicsWorkContext;
}

void VulkanRHLCommandList::Submit() const {
    Photon::Result finalizeResult = m_rhlGraphicsWorkContext->FinalizeWork();
}

void VulkanRHLCommandList::Transition(const VulkanRHLResourceTransitionRequestBatch &transitionRequests) const {
    VulkanRHLPendingTransition pendingTransition = g_vulkanDynamicRHL->CreateTransition(transitionRequests);
    GetWorkContext(pendingTransition.GetSrcWorkType())->BeginTransition(pendingTransition);
    GetWorkContext(pendingTransition.GetDestWorkType())->EndTransition(pendingTransition);
}

void VulkanRHLCommandList::Transition(VulkanRHLTextureTransition &&textureTransition) const {
    VulkanRHLResourceTransitionRequestBatch transitionBatch{textureTransition.m_srcWorkType, textureTransition.m_destWorkType};
    transitionBatch.AddTextureTransitionRequest({textureTransition.m_rhlTexture});
    Transition(transitionBatch);
}

void VulkanRHLCommandList::AddGeneralWorkContext(VulkanRHLCommandWorkContext *workContext) {
    m_workContexts[static_cast<uint32_t>(workContext->GetVulkanRHLWorkType())] = workContext;
}

VulkanRHLCommandWorkContext* VulkanRHLCommandList::GetWorkContext(RHLWorkType rhlWork) const {
    uint32_t rawRHlWork = static_cast<uint32_t>(rhlWork);
    if (rawRHlWork >= WorkTypeCount)
        return nullptr;

    return m_workContexts[rawRHlWork];
}