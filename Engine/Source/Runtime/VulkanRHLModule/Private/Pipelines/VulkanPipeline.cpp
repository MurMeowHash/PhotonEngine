#include "../../Public/Pipelines/VulkanPipeline.h"

bool VulkanPipeline::Create(const VulkanPipelineCreateInfo& createInfo) {

}

vk::raii::Pipeline & VulkanPipeline::GetHandle() const {
    return m_handle;
}
