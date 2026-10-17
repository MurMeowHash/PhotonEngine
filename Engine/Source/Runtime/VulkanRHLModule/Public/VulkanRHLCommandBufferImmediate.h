#pragma once

#include "VulkanRHLCommandBuffer.h"

struct VulkanRHLCommandBufferImmediateCreateInfo {

};

class VulkanRHLCommandBufferImmediate : public VulkanRHLCommandBuffer {
public:
    static VulkanRHLCommandBufferImmediate* Create([[maybe_unused]] const VulkanRHLCommandBufferImmediateCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    [[nodiscard]] bool BeginRenderingViewport(VulkanRHLViewport* rhlViewport);
    void EndRenderingViewport(VulkanRHLViewport* rhlViewport);
};
