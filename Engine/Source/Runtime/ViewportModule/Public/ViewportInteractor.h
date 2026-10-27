#pragma once

#include <cstdint>
#include <RenderData/RenderSceneInput.h>

class VulkanRHLBuffer;

class Viewport;
class VulkanRHLCommandBufferImmediate;

class ViewportInteractor {
public:
    ~ViewportInteractor();
    explicit ViewportInteractor(VulkanRHLCommandBufferImmediate* rhlCommandBuffer);
    void ConnectViewport(Viewport* viewport);
    void RenderViewport(VulkanRHLCommandBufferImmediate* rhlCommandBuffer) const;
    void ChangeViewportSize(uint32_t windowWidth, uint32_t windowHeight);
private:
    Viewport* m_viewport = nullptr;
    VulkanRHLBuffer* m_dummyObjectVertexBuffer = nullptr;
    VulkanRHLBuffer* m_dummyObjectIndexBuffer = nullptr;
    VulkanRHLVertexInput* m_dummyObjectVertexInput{};
    uint32_t m_dummyObjectIndexCount{};

    void CreateDummyRenderObject(VulkanRHLCommandBufferImmediate* rhlCommandBuffer);
};