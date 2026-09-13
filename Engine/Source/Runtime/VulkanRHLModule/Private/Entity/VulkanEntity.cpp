#include "../../Public/Entity/VulkanEntity.h"
#include "CoreUtils.h"

void VulkanEntity::SetApiVersion(uint32_t apiVersion) {
    m_apiVersion = apiVersion;
}

uint32_t VulkanEntity::GetApiVersion() const {
    return m_apiVersion;
}

bool VulkanEntity::IsExtensionSupported(const char *extensionName) const {
    return IsExtensionSupported(VulkanExtension::EncodeExtensionName(extensionName));
}

bool VulkanEntity::IsExtensionSupported(VulkanExtension::EncodingType extensionEncoding) const {
    auto extensionIterator = m_extensions.find(extensionEncoding);
    if(extensionIterator == m_extensions.end())
        return false;

    return extensionIterator->second.IsSupported();
}

std::vector<const char*> VulkanEntity::QueryExtensionNames(VulkanExtensionQueryFilter queryFilter, const std::unordered_set<const char*>* excludedExtensions) const {
    std::vector<const char*> extensionNames;
    extensionNames.reserve(m_extensions.size());

    if (queryFilter == VulkanExtensionQueryFilter::None)
        return extensionNames;

    for (const auto& [encoding, extension] : m_extensions) {
        if (excludedExtensions && excludedExtensions->contains(extension.GetExtensionName()))
            continue;

        bool matches = false;

        if (Photon::Core::Flags::IsFlagSet(queryFilter, VulkanExtensionQueryFilter::Supported))
            matches |= extension.IsSupported();

        if (Photon::Core::Flags::IsFlagSet(queryFilter, VulkanExtensionQueryFilter::Required))
            matches |= extension.IsRequired();

        if (Photon::Core::Flags::IsFlagSet(queryFilter, VulkanExtensionQueryFilter::Unsupported))
            matches |= !extension.IsSupported();

        if (matches)
            extensionNames.emplace_back(extension.GetExtensionName());
    }

    return extensionNames;
}

void VulkanEntity::ProcessExtensionsRequest(const std::vector<VulkanExtension> &requestedExtensions) {
    std::vector<VulkanExtension> filteredExtensions = requestedExtensions;
    ValidateExtensions(filteredExtensions);

    m_extensions.clear();
    for (const VulkanExtension& extension : filteredExtensions) {
        m_extensions.emplace(extension.GetExtensionEncoding(), extension);
    }
}

void VulkanEntity::ValidateExtensions(std::vector<VulkanExtension> &requestedExtensions) const {
    std::vector<vk::ExtensionProperties> availableExtensions;
    if (!TryGetAvailableExtensions(availableExtensions))
        return;

    for(VulkanExtension& requestedExtension : requestedExtensions) {
        if(std::ranges::any_of(availableExtensions, [&requestedExtension](const vk::ExtensionProperties& supportedExtension){
            return std::strcmp(requestedExtension.GetExtensionName(), supportedExtension.extensionName) == 0;
        }))
            requestedExtension.MarkSupported();
        else
            requestedExtension.MarkUnsupported();
    }
}
