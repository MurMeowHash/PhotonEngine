#include "../../Public/Device/VulkanDeviceProvider.h"
#include <unordered_map>
#include "CoreGlobals.h"
#include "CoreUtils.h"

namespace Photon::Vulkan::DeviceProvider {
    std::unordered_map<vk::PhysicalDeviceType, int> g_deviceTypePriorityMap = {
        {vk::PhysicalDeviceType::eDiscreteGpu, 0},
        {vk::PhysicalDeviceType::eIntegratedGpu, 1},
        {vk::PhysicalDeviceType::eCpu, 2},
        {vk::PhysicalDeviceType::eVirtualGpu, 3},
        {vk::PhysicalDeviceType::eOther, 4}
    };

    struct VulkanDeviceInfo {
        vk::raii::PhysicalDevice m_physicalDevice;
        vk::PhysicalDeviceProperties2 m_deviceProperties;
        std::vector<vk::QueueFamilyProperties2> m_queueFamilyProperties;
    };

    [[nodiscard]] uint64_t EncodeDeviceId(uint32_t vendorID, uint32_t deviceID) {
        return static_cast<uint64_t>(vendorID) << sizeof(vendorID) |
            static_cast<uint64_t>(deviceID);
    }

    [[nodiscard]] bool IsGpuType(vk::PhysicalDeviceType physicalDeviceType) {
        return physicalDeviceType == vk::PhysicalDeviceType::eDiscreteGpu
        || physicalDeviceType == vk::PhysicalDeviceType::eIntegratedGpu;
    }

    [[nodiscard]] bool IsSupportsGraphics(const std::vector<vk::QueueFamilyProperties2> &deviceQueueFamilies) {
        return std::ranges::any_of(deviceQueueFamilies,
            [](const vk::QueueFamilyProperties2& queueFamilyProperties){
                return static_cast<bool>(queueFamilyProperties.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics);
        });
    }

    bool TryFindVulkanDevice(const vk::raii::Instance &vkInstance, uint64_t predefinedDeviceId, DeviceSearchFlags flags, vk::raii::PhysicalDevice *physicalDevice) {
        if(physicalDevice == nullptr)
            return false;

        vk::ResultValue<std::vector<vk::raii::PhysicalDevice>> availableDevices = vkInstance.enumeratePhysicalDevices();
        if (availableDevices.result != vk::Result::eSuccess || availableDevices.value.empty())
            return false;

        std::vector<VulkanDeviceInfo> vulkanDevices;
        vulkanDevices.reserve(availableDevices.value.size());

        for(const vk::raii::PhysicalDevice& device : availableDevices.value) {
            vulkanDevices.emplace_back(VulkanDeviceInfo(device,
                                                        device.getProperties2(),
                                                        device.getQueueFamilyProperties2()));
        }

        std::ranges::sort(vulkanDevices,[](const VulkanDeviceInfo& lhs, const VulkanDeviceInfo& rhs) {
            return g_deviceTypePriorityMap.find(lhs.m_deviceProperties.properties.deviceType)->second
                   < g_deviceTypePriorityMap.find(rhs.m_deviceProperties.properties.deviceType)->second;
        });

        int chosenDeviceIndex = Photon::Core::InvalidIndex;
        for(size_t i = 0; i < vulkanDevices.size(); i++) {
            const vk::PhysicalDeviceProperties& deviceProperties = vulkanDevices[i].m_deviceProperties.properties;
            const std::vector<vk::QueueFamilyProperties2>& queueFamilyProperties = vulkanDevices[i].m_queueFamilyProperties;

            if(predefinedDeviceId == EncodeDeviceId(deviceProperties.vendorID, deviceProperties.deviceID))
                chosenDeviceIndex = static_cast<int>(i);

            if(chosenDeviceIndex == Photon::Core::InvalidIndex) {
                bool physicalDeviceCompatible = IsGpuType(deviceProperties.deviceType)
                        || Photon::Core::Flags::IsFlagSet(flags, DeviceSearchFlags::AllowNonGpu);
                bool graphicsCompatible = !Photon::Core::Flags::IsFlagSet(flags, DeviceSearchFlags::RenderingOnly)
                                          || IsSupportsGraphics(queueFamilyProperties);
                if(physicalDeviceCompatible && graphicsCompatible)
                    chosenDeviceIndex = static_cast<int>(i);
            }
        }

        if(chosenDeviceIndex == Photon::Core::InvalidIndex)
            return false;

        *physicalDevice = vulkanDevices[chosenDeviceIndex].m_physicalDevice;
        return true;
    }

    uint64_t GetDeviceIdByName(const vk::raii::Instance &vkInstance, const char *physicalDeviceName) {
        vk::ResultValue<std::vector<vk::raii::PhysicalDevice> > availableDevices = vkInstance.enumeratePhysicalDevices();
        if (availableDevices.result != vk::Result::eSuccess)
            return 0u;

        for(const vk::raii::PhysicalDevice& device : availableDevices.value) {
            vk::PhysicalDeviceProperties2 deviceProperties = device.getProperties2();
            if(std::strcmp(deviceProperties.properties.deviceName, physicalDeviceName) == 0)
                return EncodeDeviceId(deviceProperties.properties.vendorID, deviceProperties.properties.deviceID);
        }

        return 0u;
    }
}
