#include "../../Public/Device/VulkanDeviceFactory.h"

VulkanDevice * Photon::Vulkan::DeviceFactory::CreateVulkanDevice(const VulkanDeviceCreateInfo &createInfo, bool *isValid) {
    VulkanDevice* device = new VulkanDevice();
    bool isCreated = device->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return device;
}