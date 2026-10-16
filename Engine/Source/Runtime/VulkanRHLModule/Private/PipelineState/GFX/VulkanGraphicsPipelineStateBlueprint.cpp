#include "../../../Public/PipelineState/GFX/VulkanGraphicsPipelineStateBlueprint.h"
#include "PipelineState/VulkanPipelineStateProperty.h"

void VulkanGraphicsPipelineStateBlueprint::InstallProperties() {
    m_blueprintContainer.BindProperty<VulkanPipelineStateProperty>().WithArguments<>();
}