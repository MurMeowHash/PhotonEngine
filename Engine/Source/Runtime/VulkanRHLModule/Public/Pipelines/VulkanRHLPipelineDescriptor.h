#pragma once

#include "Blueprints/BlueprintDescriptor.h"
#include "Shaders/VulkanRHLShader.h"

class VulkanGraphicsPipelineProvider;

struct VulkanRHLVertexShadingStage {
    VulkanRHLVertexShader* m_rhlVertexShader = nullptr;

    bool operator==(const VulkanRHLVertexShadingStage&) const = default;

    VulkanRHLVertexShadingStage& SetVertexShader(VulkanRHLVertexShader* rhlVertexShader) {
        m_rhlVertexShader = rhlVertexShader;
        return *this;
    }
};

struct VulkanRHLPrimitiveAssemblyStage {
    Photon::Vulkan::RHLPrimitiveType m_primitiveType{};

    bool operator==(const VulkanRHLPrimitiveAssemblyStage&) const = default;

    VulkanRHLPrimitiveAssemblyStage& SetPrimitiveType(Photon::Vulkan::RHLPrimitiveType primitiveType) {
        m_primitiveType = primitiveType;
        return *this;
    }
};

struct VulkanRHLFragmentShadingStage {
    VulkanRHLFragmentShader* m_rhlFragmentShader = nullptr;

    bool operator==(const VulkanRHLFragmentShadingStage&) const = default;

    VulkanRHLFragmentShadingStage& SetFragmentShader(VulkanRHLFragmentShader* rhlFragmentShader) {
        m_rhlFragmentShader = rhlFragmentShader;
        return *this;
    }
};

struct VulkanRHLPresentationStage {
    std::vector<Photon::Vulkan::RHLFormat> m_colorAttachments;

    bool operator==(const VulkanRHLPresentationStage&) const = default;

    VulkanRHLPresentationStage& AddColorAttachment(Photon::Vulkan::RHLFormat colorAttachment) {
        m_colorAttachments.push_back(colorAttachment);
        return *this;
    }
};

class VulkanRHLGraphicsPipelineDescriptor : public BlueprintDescriptor {
    friend class VulkanGraphicsPipelineProvider;
public:
    [[nodiscard]] size_t GetIdentifier() const override {
        size_t seed = 0;

        auto hashCombine = [&seed](size_t value) {
            seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };

        hashCombine(std::hash<const void*>{}(
            m_vertexShadingStage.m_rhlVertexShader));

        hashCombine(std::hash<Photon::Vulkan::RHLPrimitiveType>{}(
            m_primitiveAssemblyStage.m_primitiveType));

        hashCombine(std::hash<const void*>{}(
            m_fragmentShadingStage.m_rhlFragmentShader));

        hashCombine(std::hash<size_t>{}(
            m_presentationStage.m_colorAttachments.size()));
        for (Photon::Vulkan::RHLFormat colorAttachment : m_presentationStage.m_colorAttachments)
            hashCombine(std::hash<Photon::Vulkan::RHLFormat>{}(colorAttachment));

        return seed;
    }

    bool operator==(const VulkanRHLGraphicsPipelineDescriptor& other) const {
        return m_vertexShadingStage == other.m_vertexShadingStage
            && m_primitiveAssemblyStage == other.m_primitiveAssemblyStage
            && m_fragmentShadingStage == other.m_fragmentShadingStage
            && m_presentationStage == other.m_presentationStage;
    }

public:
    VulkanRHLGraphicsPipelineDescriptor& AddVertexStage(const VulkanRHLVertexShadingStage& vertexShadingStage) {
        m_vertexShadingStage = vertexShadingStage;
        return *this;
    }

    VulkanRHLGraphicsPipelineDescriptor& AddPrimitiveAssemblyStage(const VulkanRHLPrimitiveAssemblyStage& primitiveAssemblyStage) {
        m_primitiveAssemblyStage = primitiveAssemblyStage;
        return *this;
    }

    VulkanRHLGraphicsPipelineDescriptor& AddFragmentStage(const VulkanRHLFragmentShadingStage& fragmentShadingStage) {
        m_fragmentShadingStage = fragmentShadingStage;
        return *this;
    }

    VulkanRHLGraphicsPipelineDescriptor& AddPresentationStage(const VulkanRHLPresentationStage& presentationStage) {
        m_presentationStage = presentationStage;
        return *this;
    }

private:
    VulkanRHLVertexShadingStage m_vertexShadingStage;
    VulkanRHLPrimitiveAssemblyStage m_primitiveAssemblyStage;
    VulkanRHLFragmentShadingStage m_fragmentShadingStage;
    VulkanRHLPresentationStage m_presentationStage;
};
