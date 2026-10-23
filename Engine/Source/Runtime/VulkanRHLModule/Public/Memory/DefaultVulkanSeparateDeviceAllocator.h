#pragma once

#include "IVulkanSeparateDeviceAllocator.h"
#include "VulkanMemoryPage.h"

class VulkanDevice;

struct DefaultVulkanSeparateDeviceAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
};

class DefaultVulkanSeparateDeviceAllocator : public IVulkanSeparateDeviceAllocator {
public:
    explicit DefaultVulkanSeparateDeviceAllocator(const DefaultVulkanSeparateDeviceAllocatorCreateInfo& createInfo);

    [[nodiscard]] VulkanDeviceMemory AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment) override;
    [[nodiscard]] vk::DeviceSize GetMaxAllocationSize() const override;
    [[nodiscard]] VulkanDeviceMemory AllocateImageMemoryVkDedicated(vk::Image image) override;
    [[nodiscard]] VulkanDeviceMemory AllocateBufferMemoryVkDedicated(vk::Buffer buffer) override;
    void FreeMemory(VulkanDeviceMemory &memory) override;
    void FreeAllMemory() override;

private:
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
    VulkanDeviceMemoryPagesAllocatorSeparate m_memoryPool;
};
