#pragma once

#include "Shaders/Shader.h"

class DummyTriangleVS : public Shader<VulkanRHLVertexShader> {
    DECLARE_GLOBAL_SHADER()
};

class DummyTriangleFS : public Shader<VulkanRHLFragmentShader> {
    DECLARE_GLOBAL_SHADER()
};