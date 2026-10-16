#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

struct VulkanPipelineResourceTransitionScope;

class VulkanRHLResource {
public:
    virtual ~VulkanRHLResource() = default;
    virtual void CommitPendingTransitionScope() = 0;
    [[nodiscard]] vk::PipelineStageFlags2 GetLastPipelineStage() const;
    [[nodiscard]] vk::AccessFlags2 GetLastAccess() const;
protected:
    vk::PipelineStageFlags2 m_lastPipelineStage{};
    vk::AccessFlags2 m_lastAccess{};
    uint32_t m_lastQueueFamily{};

    void CommitResourceTransitionScope(const VulkanPipelineResourceTransitionScope& transitionScope);
};