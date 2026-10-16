#pragma once

#include "RenderTarget.h"
#include <vector>

class VulkanRHLVertexShader;
class VulkanRHLFragmentShader;
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
