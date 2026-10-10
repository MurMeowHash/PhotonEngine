#include "../Public/VulkanRHLCommandList.h"

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
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanRHLGraphicsWorkContext * VulkanRHLCommandList::GetGraphicsContext() const {
    return m_rhlGraphicsWorkContext;
}

void VulkanRHLCommandList::Submit() const {
    Photon::Result finalizeResult = m_rhlGraphicsWorkContext->FinalizeWork();
}
