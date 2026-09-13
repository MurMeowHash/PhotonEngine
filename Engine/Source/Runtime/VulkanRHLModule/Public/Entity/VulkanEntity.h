#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include "VulkanExtension.h"
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include <unordered_set>

enum VulkanExtensionQueryFilter : uint32_t {
    None = 0,
    Supported = 1 << 0,
    Required = 1 << 1,
    Unsupported = 1 << 2,
    All = Supported | Required | Unsupported
};

class VulkanEntity {
public:
    virtual ~VulkanEntity() = default;

    void SetApiVersion(uint32_t apiVersion);
    [[nodiscard]] uint32_t GetApiVersion() const;
    [[nodiscard]] bool IsExtensionSupported(const char *extensionName) const;
    [[nodiscard]] bool IsExtensionSupported(VulkanExtension::EncodingType extensionEncoding) const;
    [[nodiscard]] virtual bool TryGetAvailableExtensions(std::vector<vk::ExtensionProperties>& availableExtensions) const = 0;
    [[nodiscard]] std::vector<const char *> QueryExtensionNames(VulkanExtensionQueryFilter queryFilter,
        const std::unordered_set<const char*>* excludedExtensions = nullptr) const;

protected:
    void ProcessExtensionsRequest(const std::vector<VulkanExtension>& requestedExtensions);

private:
    uint32_t m_apiVersion = 0;
    std::unordered_map<VulkanExtension::EncodingType, VulkanExtension> m_extensions;

    void ValidateExtensions(std::vector<VulkanExtension>& requestedExtensions) const;
};
