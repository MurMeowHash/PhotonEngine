#include "../../Public/Pipelines/VulkanGraphicsPipeline.h"
#include "Device/VulkanDevice.h"

bool VulkanGraphicsPipeline::Create(const VulkanGraphicsPipelineCreateInfo &createInfo) {
    std::vector<vk::PipelineShaderStageCreateInfo> shaderModulesInfo = AssembleShaderPipelineData(createInfo.m_pipelineDesc);
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, createInfo.m_pipelineDesc.m_primitiveAssemblyStage.m_primitiveTopology);
    vk::PipelineViewportStateCreateInfo viewportInfo({}, 1, {}, 1, {});
    VulkanRasterizationStage rasterizationStageSrc = createInfo.m_pipelineDesc.m_rasterizationStage;
    vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
    rasterizationInfo.polygonMode = rasterizationStageSrc.m_polygonMode;
    rasterizationInfo.cullMode = rasterizationStageSrc.m_cullModeFlags;
    vk::PipelineMultisampleStateCreateInfo multisampleInfo;
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendAttachment;

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates.size(), dynamicStates.data());

    bool pipelineLayoutCreated = CreatePipelineLayout(createInfo.m_vulkanDevice->GetHandle());
    if (!pipelineLayoutCreated)
        return false;

    VulkanPresentationStage presentationStage = createInfo.m_pipelineDesc.m_presentationStage;
    vk::PipelineRenderingCreateInfo renderingInfo(
        {},
        presentationStage.m_colorAttachments.size(),
        presentationStage.m_colorAttachments.data(),
        presentationStage.m_depthAttachment);

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.stageCount = shaderModulesInfo.size();
    graphicsPipelineCreateInfo.pStages = shaderModulesInfo.data();
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateInfo;
    graphicsPipelineCreateInfo.layout = m_pipelineLayout;

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfo = {
        graphicsPipelineCreateInfo, renderingInfo
    };

    vk::ResultValue<vk::raii::Pipeline> graphicsPipelineWrapper = createInfo.m_vulkanDevice->GetHandle().createGraphicsPipeline(
        nullptr, pipelineCreateInfo.get<vk::GraphicsPipelineCreateInfo>());

    if (graphicsPipelineWrapper.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(graphicsPipelineWrapper.value);
    return true;
}

bool VulkanGraphicsPipeline::CreatePipelineLayout(const vk::raii::Device& device) {
    vk::PipelineLayoutCreateInfo layoutInfo;
    vk::ResultValue<vk::raii::PipelineLayout> pipelineLayoutWrapper = device.createPipelineLayout(layoutInfo);
    if (pipelineLayoutWrapper.result != vk::Result::eSuccess)
        return false;

    m_pipelineLayout = std::move(pipelineLayoutWrapper.value);
    return true;
}

std::vector<vk::PipelineShaderStageCreateInfo> VulkanGraphicsPipeline::AssembleShaderPipelineData(const VulkanGraphicsPipelineDescriptor &pipelineDesc) {
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos;
    AddShaderToAssembleData(pipelineDesc.m_vertexShadingStage.m_shaderModule, shaderStageCreateInfos);
    AddShaderToAssembleData(pipelineDesc.m_fragmentShadingStage.m_shaderModule, shaderStageCreateInfos);
    return shaderStageCreateInfos;
}

void VulkanGraphicsPipeline::AddShaderToAssembleData(VulkanShaderModule *vulkanShader, std::vector<vk::PipelineShaderStageCreateInfo> &assembledShaders) {
    vk::PipelineShaderStageCreateInfo vertexShaderCreateInfo({}, vulkanShader->GetStage(), vulkanShader->GetHandle(), vulkanShader->GetEntryName());
    assembledShaders.emplace_back(vertexShaderCreateInfo);
}