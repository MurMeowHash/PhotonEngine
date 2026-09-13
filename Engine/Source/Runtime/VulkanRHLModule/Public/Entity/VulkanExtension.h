#pragma once

#include <cstddef>

class VulkanExtension {
public:
    using EncodingType = size_t;
    VulkanExtension(const char* extensionName, bool required);
    explicit VulkanExtension() = default;
    void MarkSupported();
    void MarkUnsupported();
    [[nodiscard]] bool IsSupported() const;
    [[nodiscard]] const char* GetExtensionName() const;
    [[nodiscard]] size_t GetExtensionEncoding() const;
    [[nodiscard]] bool IsRequired() const;

    static size_t EncodeExtensionName(const char* extensionName);
private:
    const char* m_extensionName = "";
    bool m_required = false;
    bool m_supported = false;
    size_t m_encoding = 0;
};