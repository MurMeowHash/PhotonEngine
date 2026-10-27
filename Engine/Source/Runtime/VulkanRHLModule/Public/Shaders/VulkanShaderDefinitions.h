#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

enum class VulkanRHLShaderSemanticType {
    Undefined = 0,
    Position = 1,
};

enum class VulkanRHLShaderFormat {
    Undefined = 0,
    R32G32_Float = 1,
};

enum class VulkanRHLShaderStage {
    None = 0,
    Vertex = 1,
    Fragment = 2,
};

struct VulkanRHLShaderSemantic {
    VulkanRHLShaderSemanticType m_semanticType;
    uint32_t m_semanticIndex;

    [[nodiscard]] bool IsValid() const {
        return m_semanticType != VulkanRHLShaderSemanticType::Undefined;
    }

    auto operator<=>(const VulkanRHLShaderSemantic&) const = default;
    bool operator==(const VulkanRHLShaderSemantic&) const = default;
};

template<>
struct std::hash<VulkanRHLShaderSemantic> {
    size_t operator()(const VulkanRHLShaderSemantic& semantic) const noexcept {
        size_t seed = std::hash<VulkanRHLShaderSemanticType>{}(semantic.m_semanticType);
        seed ^= std::hash<uint32_t>{}(semantic.m_semanticIndex) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

struct VulkanShaderVertexAttribute {
    VulkanRHLShaderSemantic m_semantic;
    uint32_t m_location;
    VulkanRHLShaderFormat m_format;
};

struct VulkanShaderMetaData {
    std::vector<VulkanShaderVertexAttribute> m_vertexAttributes;
};