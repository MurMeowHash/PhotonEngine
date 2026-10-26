#include "../../../Public/PipelineState/GFX/VulkanGraphicsPipelineStateBlueprint.h"
#include "PipelineState/GFX/VulkanGraphicsPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanViewportPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanScissorPipelineStateProperty.h"
#include "PipelineState/GFX/VulkanVertexStreamPipelineStateProperty.h"

void VulkanGraphicsPipelineStateBlueprint::InstallProperties() {
    m_blueprintContainer.BindProperty<VulkanGraphicsPipelineStateProperty>().WithArguments<>();
    m_blueprintContainer.BindProperty<VulkanViewportPipelineStateProperty>().WithArguments<>();
    m_blueprintContainer.BindProperty<VulkanScissorPipelineStateProperty>().WithArguments<>();
    m_blueprintContainer.BindProperty<VulkanVertexStreamPipelineStateProperty>().WithArguments<VulkanGraphicsPipelineStateProperty>();
}