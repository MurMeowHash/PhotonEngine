#include "../../Public/VertexFactory/VulkanRHLVertexLayout.h"
#include <algorithm>
#include <tuple>

size_t VulkanRHLVertexLayoutDescriptor::GetIdentifier() const {
    size_t seed = 0;

    auto hashCombine = [&seed](size_t value) {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };

    hashCombine(std::hash<size_t>{}(m_vertexDescriptions.size()));
    for (const VulkanRHLVertexDescription& vertexDescription : m_vertexDescriptions) {
        hashCombine(std::hash<VulkanRHLShaderSemantic>{}(vertexDescription.m_semantic));
        hashCombine(std::hash<VulkanRHLFormat>{}(vertexDescription.m_format));
        hashCombine(std::hash<uint32_t>{}(vertexDescription.m_streamSource));
        hashCombine(std::hash<uint32_t>{}(vertexDescription.m_streamOffset));
    }

    hashCombine(std::hash<size_t>{}(m_streamDescriptions.size()));
    for (const VulkanRHLStreamDescription& streamDescription : m_streamDescriptions) {
        hashCombine(std::hash<uint32_t>{}(streamDescription.m_streamSource));
        hashCombine(std::hash<uint32_t>{}(streamDescription.m_vertexStride));
        hashCombine(std::hash<VulkanRHLVertexInputRate>{}(streamDescription.m_vertexInputRate));
    }

    return seed;
}

void VulkanRHLVertexLayoutDescriptor::AddVertexDescription(const VulkanRHLVertexDescription &vertexDescription) {
    auto vertexDescriptionKey = [](const VulkanRHLVertexDescription& description) {
        return std::tie(description.m_streamSource, description.m_streamOffset, description.m_semantic,
            description.m_format);
    };

    auto insertPosition = std::ranges::upper_bound(m_vertexDescriptions, vertexDescription,
        [&vertexDescriptionKey](const VulkanRHLVertexDescription& lhs, const VulkanRHLVertexDescription& rhs) {
            return vertexDescriptionKey(lhs) < vertexDescriptionKey(rhs);
        });
    m_vertexDescriptions.insert(insertPosition, vertexDescription);
}

void VulkanRHLVertexLayoutDescriptor::AddStreamDescription(const VulkanRHLStreamDescription &streamDescription) {
    auto streamDescriptionKey = [](const VulkanRHLStreamDescription& description) {
        return std::tie(description.m_streamSource, description.m_vertexStride, description.m_vertexInputRate);
    };

    auto insertPosition = std::ranges::upper_bound(m_streamDescriptions, streamDescription,
        [&streamDescriptionKey](const VulkanRHLStreamDescription& lhs, const VulkanRHLStreamDescription& rhs) {
            return streamDescriptionKey(lhs) < streamDescriptionKey(rhs);
        });
    m_streamDescriptions.insert(insertPosition, streamDescription);
}

const std::vector<VulkanRHLVertexDescription>& VulkanRHLVertexLayoutDescriptor::GetVertexDescriptions() const {
    return m_vertexDescriptions;
}

const std::vector<VulkanRHLStreamDescription>& VulkanRHLVertexLayoutDescriptor::GetStreamDescriptions() const {
    return m_streamDescriptions;
}

bool VulkanRHLVertexLayoutDescriptor::operator==(const VulkanRHLVertexLayoutDescriptor &other) const {
    return m_vertexDescriptions == other.m_vertexDescriptions
        && m_streamDescriptions == other.m_streamDescriptions;
}

VulkanRHLVertexLayout* VulkanRHLVertexLayout::Create(const VulkanRHLVertexLayoutCreateInfo &createInfo,
                                                     InOutCreateParams<Photon::Result> *inOutCreateParams) {
    for (const VulkanRHLVertexDescription& vertexDesc : createInfo.m_layoutDescriptor.GetVertexDescriptions()) {
        const bool isStreamForVertexSpecified =
            std::ranges::any_of(createInfo.m_layoutDescriptor.GetStreamDescriptions(), [&vertexDesc](const VulkanRHLStreamDescription& streamDesc) {
                return streamDesc.m_streamSource == vertexDesc.m_streamSource;
        });

        if (!isStreamForVertexSpecified) {
            Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
            return nullptr;
        }
    }

    VulkanRHLVertexLayout* instance = Photon::AllocateObject<VulkanRHLVertexLayout>(inOutCreateParams);
    instance->m_vertexDescriptions = createInfo.m_layoutDescriptor.GetVertexDescriptions();
    instance->m_streamDescriptions = createInfo.m_layoutDescriptor.GetStreamDescriptions();
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

const std::vector<VulkanRHLVertexDescription> & VulkanRHLVertexLayout::GetVertexDescriptions() const {
    return m_vertexDescriptions;
}

const std::vector<VulkanRHLStreamDescription> & VulkanRHLVertexLayout::GetStreamDescriptions() const {
    return m_streamDescriptions;
}

VulkanRHLVertexLayout* VulkanRHLVertexLayoutCache::CreateBlueprint(const VulkanRHLVertexLayoutDescriptor &desc, void *allocatedMemory) {
    VulkanRHLVertexLayoutCreateInfo createInfo{desc};
    InOutCreateParams<Photon::Result> inOut{};
    inOut.m_preAllocatedMemory = allocatedMemory;
    return VulkanRHLVertexLayout::Create(createInfo, &inOut);
}
