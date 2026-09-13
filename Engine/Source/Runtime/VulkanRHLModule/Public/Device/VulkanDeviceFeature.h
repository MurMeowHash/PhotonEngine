#pragma once

enum class VulkanDeviceFeatureType {
    None = 0,
    ShaderDrawParameters = 1,
    DynamicRendering = 2,
    Synchronization2 = 3,
};


class VulkanDeviceFeature {
public:
    VulkanDeviceFeature(VulkanDeviceFeatureType featureType, bool required);
    explicit VulkanDeviceFeature() = default;
    void MarkEnabled(bool enabled);
    [[nodiscard]] bool IsEnabled() const;

    [[nodiscard]] VulkanDeviceFeatureType GetFeatureType() const;
    [[nodiscard]] bool IsRequired() const;
private:
    VulkanDeviceFeatureType m_featureType = VulkanDeviceFeatureType::None;
    bool m_required = false;
    bool m_enabled = false;
};