#include "../../Public/Pipelines/VulkanPipeline.h"

vk::Pipeline VulkanPipeline::GetHandle() const {
    return *m_handle;
}