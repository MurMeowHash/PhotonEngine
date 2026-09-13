#include "../../Public/Device/VulkanDeviceFeature.h"

VulkanDeviceFeature::VulkanDeviceFeature(VulkanDeviceFeatureType featureType, bool required)
: m_featureType(featureType), m_required(required) { }

void VulkanDeviceFeature::MarkEnabled(bool enabled) {
    m_enabled = enabled;
}

bool VulkanDeviceFeature::IsEnabled() const {
    return m_enabled;
}

VulkanDeviceFeatureType VulkanDeviceFeature::GetFeatureType() const {
    return m_featureType;
}

bool VulkanDeviceFeature::IsRequired() const {
    return m_required;
}
