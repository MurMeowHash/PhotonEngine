#include "../../Public/Pipelines/VulkanPipelineProvider.h"
#include "VulkanUtils.h"

VulkanGraphicsPipelineProvider* VulkanGraphicsPipelineProvider::Create(const VulkanGraphicsPipelineProviderCreateInfo &createInfo,
    InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanGraphicsPipelineProvider* instance = Photon::AllocateObject<VulkanGraphicsPipelineProvider>(inOutCreateParams);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

VulkanGraphicsPipeline* VulkanGraphicsPipelineProvider::CreateBlueprint(const VulkanRHLGraphicsPipelineDescriptor &desc, void *allocatedMemory) {
    VulkanGraphicsPipelineCreateInfo createInfo(
        m_vulkanDevice,
        VulkanShaderPipeline({
            desc.m_vertexShadingStage.m_rhlVertexShader,
            desc.m_fragmentShadingStage.m_rhlFragmentShader
        }),
        desc.m_vertexShadingStage.m_vertexLayout,
        Photon::Vulkan::RHLPrimitiveToVulkanTopology(desc.m_primitiveAssemblyStage.m_primitiveType)
    );


    createInfo.m_colorAttachments.reserve(desc.m_presentationStage.m_colorAttachments.size());
    for (VulkanRHLFormat rhlFormat : desc.m_presentationStage.m_colorAttachments)
        createInfo.m_colorAttachments.emplace_back(Photon::Vulkan::RHLFormatToVulkan(rhlFormat));

    InOutCreateParams<Photon::Result> inOut{};
    inOut.m_preAllocatedMemory = allocatedMemory;
    return VulkanGraphicsPipeline::Create(createInfo, &inOut);
}