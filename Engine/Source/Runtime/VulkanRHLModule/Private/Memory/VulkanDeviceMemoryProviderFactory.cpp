#include "../../Public/Memory/VulkanDeviceMemoryProviderFactory.h"
#include "Memory/VulkanDeviceMemoryProvider.h"
#include <stdexcept>
#include "CoreUtils.h"

IVulkanDeviceMemoryProvider* Photon::Vulkan::DeviceMemoryProviderFactory::CreateVulkanDeviceMemoryProvider(VulkanMemoryProviderType memoryProviderType,
    vk::raii::Device &device, vk::raii::PhysicalDevice &physicalDevice, const VulkanAllocationPresetDefinition &allocationPreset) {
    switch (memoryProviderType) {
        case VulkanMemoryProviderType::Default:
            return new VulkanDeviceMemoryProvider(device, physicalDevice, allocationPreset);
        default:
            throw std::invalid_argument(StringFormatter("Memory provider with type ", static_cast<uint32_t>(memoryProviderType), " does not exist"));
    }
}