#include "../../Public/Entity/VulkanExtension.h"
#include <string_view>

VulkanExtension::VulkanExtension(const char *extensionName, bool required)
: m_extensionName(extensionName), m_required(required) {
    m_encoding = EncodeExtensionName(m_extensionName);
}

void VulkanExtension::MarkSupported() {
    m_supported = true;
}

bool VulkanExtension::IsSupported() const {
    return m_supported;
}

const char *VulkanExtension::GetExtensionName() const {
    return m_extensionName;
}

size_t VulkanExtension::GetExtensionEncoding() const {
    return m_encoding;
}

bool VulkanExtension::IsRequired() const {
    return m_required;
}

size_t VulkanExtension::EncodeExtensionName(const char *extensionName) {
    return std::hash<std::string_view>{}(extensionName);
}

void VulkanExtension::MarkUnsupported() {
    m_supported = false;
}
