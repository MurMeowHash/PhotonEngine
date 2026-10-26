#pragma once

#include "Shaders/VulkanShaderDefinitions.h"
#include "VulkanDefinitions.h"
#include <vector>
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "Blueprints/BlueprintDescriptor.h"
#include "Blueprints/BlueprintProvider.h"

struct VulkanRHLVertexDescription {
    VulkanRHLShaderSemantic m_semantic;
    VulkanRHLFormat m_format;
    uint32_t m_streamSource;
    uint32_t m_streamOffset;

    bool operator==(const VulkanRHLVertexDescription&) const = default;
};

struct VulkanRHLStreamDescription {
    uint32_t m_streamSource;
    uint32_t m_vertexStride;
    VulkanRHLVertexInputRate m_vertexInputRate;

    bool operator==(const VulkanRHLStreamDescription&) const = default;
};

class VulkanRHLVertexLayoutDescriptor : public BlueprintDescriptor {
public:
    [[nodiscard]] size_t GetIdentifier() const override;

    void AddVertexDescription(const VulkanRHLVertexDescription& vertexDescription);
    void AddStreamDescription(const VulkanRHLStreamDescription& streamDescription);

    const std::vector<VulkanRHLVertexDescription>& GetVertexDescriptions() const;
    const std::vector<VulkanRHLStreamDescription>& GetStreamDescriptions() const;

    bool operator==(const VulkanRHLVertexLayoutDescriptor& other) const;
private:
    std::vector<VulkanRHLVertexDescription> m_vertexDescriptions;
    std::vector<VulkanRHLStreamDescription> m_streamDescriptions;
};

struct VulkanRHLVertexLayoutCreateInfo {
    VulkanRHLVertexLayoutDescriptor m_layoutDescriptor;
};

class VulkanRHLVertexLayout {
public:
    static VulkanRHLVertexLayout* Create(const VulkanRHLVertexLayoutCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    const std::vector<VulkanRHLVertexDescription>& GetVertexDescriptions() const;
    const std::vector<VulkanRHLStreamDescription>& GetStreamDescriptions() const;
private:
    std::vector<VulkanRHLVertexDescription> m_vertexDescriptions;
    std::vector<VulkanRHLStreamDescription> m_streamDescriptions;
};

class VulkanRHLVertexLayoutCache : public BlueprintProvider<VulkanRHLVertexLayout, VulkanRHLVertexLayoutDescriptor> {
protected:
    VulkanRHLVertexLayout* CreateBlueprint(const VulkanRHLVertexLayoutDescriptor &desc, void *allocatedMemory) override;
};