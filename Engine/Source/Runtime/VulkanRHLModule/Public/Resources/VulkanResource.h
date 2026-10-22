#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>

struct VulkanPipelineResourceTransitionScope;

class VulkanResource {
public:
    virtual ~VulkanResource() = default;
    virtual void CommitPendingTransitionScope() = 0;
    //Doesn't work for sub resources or ranges, only for general resource
    [[nodiscard]] vk::PipelineStageFlags2 GetLastPipelineStage() const;
    [[nodiscard]] vk::AccessFlags2 GetLastAccess() const;
    [[nodiscard]] virtual bool IsDedicated() const = 0;
protected:
    vk::PipelineStageFlags2 m_lastPipelineStage{};
    vk::AccessFlags2 m_lastAccess{};
    uint32_t m_lastQueueFamily{};

    void CommitResourceTransitionScope(const VulkanPipelineResourceTransitionScope& transitionScope);
};