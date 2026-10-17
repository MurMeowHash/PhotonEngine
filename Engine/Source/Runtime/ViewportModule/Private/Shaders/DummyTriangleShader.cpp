#include "../../Public/Shaders/DummyTriangleShader.h"

IMPLEMENT_GLOBAL_SHADER(DummyTriangleVS, "Private/DummyTriangle.hlsl", "DummyTriangleVS", RHLShaderStage::Vertex)
IMPLEMENT_GLOBAL_SHADER(DummyTriangleFS, "Private/DummyTriangle.hlsl", "DummyTriangleFS", RHLShaderStage::Fragment)