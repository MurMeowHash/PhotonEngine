#include "../../Public/Memory/VulkanDeviceMemoryProviderFactory.h"
#include "Memory/VulkanDeviceMemoryProvider.h"
#include <stdexcept>
#include "CoreUtils.h"

IVulkanDeviceMemoryProvider* Photon::Vulkan::DeviceMemoryProviderFactory::CreateVulkanDeviceMemoryProvider(VulkanMemoryProviderType memoryProviderType,
    VulkanDevice* vulkanDevice, const VulkanAllocationPresetDefinition &allocationPreset, bool releaseEmptyPages) {
    switch (memoryProviderType) {
        case VulkanMemoryProviderType::Default:
            return new VulkanDeviceMemoryProvider(VulkanDeviceMemoryProviderCreateInfo{vulkanDevice, allocationPreset, releaseEmptyPages});
        default:
            throw std::invalid_argument(StringFormatter("Memory provider with type ", static_cast<uint32_t>(memoryProviderType), " does not exist"));
    }
}