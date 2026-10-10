#pragma once

#include "VulkanRHLGraphicsWorkContext.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"

struct VulkanRHLCommandListCreateInfo {

};

class VulkanRHLCommandList {
public:
    ~VulkanRHLCommandList();
    static VulkanRHLCommandList* Create([[maybe_unused]] const VulkanRHLCommandListCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    VulkanRHLGraphicsWorkContext* GetGraphicsContext() const;
    void Submit() const;
private:
    VulkanRHLGraphicsWorkContext* m_rhlGraphicsWorkContext = nullptr;
};