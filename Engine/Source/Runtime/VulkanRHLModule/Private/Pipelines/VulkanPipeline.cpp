#include "../../Public/Pipelines/VulkanPipeline.h"

const vk::raii::Pipeline & VulkanPipeline::GetHandle() const {
    return m_handle;
}
