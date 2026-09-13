#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include <unordered_map>

#include "VulkanDebugger.h"
#include "Entity/VulkanEntity.h"

enum class VulkanInstanceCreateFlags : uint32_t {
    None = 0,
    AllowDropLayers = 1,
    UseDebug = 2,
    AllowDropDebug = 3,
};

struct VulkanInstanceCreateInfo {
    std::vector<VulkanExtension> m_requestedExtensions;
    std::vector<const char*> m_requestedLayers;
    VulkanInstanceCreateFlags m_createFlags;
};

class VulkanInstance : public VulkanEntity {
public:
    ~VulkanInstance() override;
    [[nodiscard]] bool TryGetAvailableExtensions(std::vector<vk::ExtensionProperties> &availableExtensions) const override;
public:
    [[nodiscard]] bool Create(const VulkanInstanceCreateInfo& createInfo);
    [[nodiscard]] const vk::raii::Instance& GetHandle() const;
private:
    vk::raii::Context m_vulkanContext;
    vk::raii::Instance m_handle = nullptr;
    VulkanDebugger* m_vulkanDebugger = nullptr;

    [[nodiscard]] bool CreateInstance(const VulkanInstanceCreateInfo &createInfo);
    [[nodiscard]] std::vector<const char*> FilterValidationLayers(const std::vector<const char*>& requestedLayers, uint32_t& droppedLayersCount) const;
};