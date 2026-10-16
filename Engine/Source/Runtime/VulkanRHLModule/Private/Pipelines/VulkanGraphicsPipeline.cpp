#include "../../Public/Pipelines/VulkanGraphicsPipeline.h"
#include "Device/VulkanDevice.h"

VulkanGraphicsPipeline* VulkanGraphicsPipeline::Create(const VulkanGraphicsPipelineCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    std::vector<vk::PipelineShaderStageCreateInfo> shaderModulesInfo = AssembleShaderPipelineData(createInfo.m_shaders);
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, createInfo.m_primitiveTopology);
    vk::PipelineViewportStateCreateInfo viewportInfo{};
    vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationInfo.cullMode = vk::CullModeFlagBits::eBack;
    vk::PipelineMultisampleStateCreateInfo multisampleInfo{};
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendAttachment;

    const std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewportWithCount,
        vk::DynamicState::eScissorWithCount,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates.size(), dynamicStates.data());
    vk::PipelineLayoutCreateInfo layoutInfo{};
    vk::ResultValue<vk::raii::PipelineLayout> pipelineLayoutWrapper = createInfo.m_vulkanDevice->GetHandle().createPipelineLayout(layoutInfo);
    if (pipelineLayoutWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    vk::PipelineRenderingCreateInfo renderingInfo(
        {},
        createInfo.m_colorAttachments.size(),
        createInfo.m_colorAttachments.data()
        );

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.stageCount = shaderModulesInfo.size();
    graphicsPipelineCreateInfo.pStages = shaderModulesInfo.data();
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayoutWrapper.value;

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfo = {
        graphicsPipelineCreateInfo, renderingInfo
    };

    vk::ResultValue<vk::raii::Pipeline> graphicsPipelineWrapper = createInfo.m_vulkanDevice->GetHandle().createGraphicsPipeline(
        nullptr, pipelineCreateInfo.get<vk::GraphicsPipelineCreateInfo>());

    if (graphicsPipelineWrapper.result != vk::Result::eSuccess) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanGraphicsPipeline* instance = Photon::AllocateObject<VulkanGraphicsPipeline>(inOutCreateParams);
    instance->m_handle = std::move(graphicsPipelineWrapper.value);
    instance->m_pipelineLayout = std::move(pipelineLayoutWrapper.value);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

std::vector<vk::PipelineShaderStageCreateInfo> VulkanGraphicsPipeline::AssembleShaderPipelineData(const std::vector<VulkanRHLShader *>& shaders) {
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos;
    shaderStageCreateInfos.reserve(shaders.size());
    for (VulkanRHLShader* rhlShader : shaders) {
        shaderStageCreateInfos.emplace_back(vk::PipelineShaderStageCreateInfo{{}, rhlShader->GetVulkanShaderStage(),
            rhlShader->GetHandle(), rhlShader->GetMetaData().m_entryName});
    }

    return shaderStageCreateInfos;
}