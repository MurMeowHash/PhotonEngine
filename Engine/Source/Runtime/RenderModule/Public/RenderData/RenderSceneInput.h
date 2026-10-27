#pragma once

#include "RenderTarget.h"
#include <cstdint>

class VulkanRHLVertexShader;
class VulkanRHLFragmentShader;
class VulkanRHLVertexInput;
class VulkanRHLBuffer;

namespace Photon::Vulkan {
    enum class RHLPrimitiveType;
}

struct RenderRect {
    float m_xMin;
    float m_xMax;
    float m_yMin;
    float m_yMax;
};

struct RenderView {
    RenderRect m_renderRect;
};

struct RenderViewCollection {
    RenderView m_renderView;
};

struct RenderObject {
    VulkanRHLVertexInput* m_vertexInput;
    VulkanRHLBuffer* m_indexBuffer;
    uint32_t m_indexCount;
    VulkanRHLVertexShader* m_vertexShader;
    VulkanRHLFragmentShader* m_fragmentShader;
    Photon::Vulkan::RHLPrimitiveType m_primitiveType;
};

struct RenderScene {
    RenderObject m_renderObject;
};

struct RenderSceneInput {
    RenderTarget* m_renderTarget;
    RenderViewCollection m_renderViewCollection;
    RenderScene m_renderScene;
};
