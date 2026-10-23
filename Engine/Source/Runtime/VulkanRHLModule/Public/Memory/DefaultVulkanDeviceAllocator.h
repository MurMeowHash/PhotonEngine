#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include "IVulkanDeviceAllocator.h"
#include "VulkanMemoryHeap.h"
#include "VulkanMemoryPage.h"

class VulkanDevice;

struct DefaultVulkanDeviceAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
    const VulkanMemoryHeap& m_vulkanMemoryHeap;
    uint32_t m_memoryTypeIndex;
    bool m_releaseEmptyPages;
};

class DefaultVulkanDeviceAllocator : public IVulkanDeviceAllocator {
public:
    explicit DefaultVulkanDeviceAllocator(const DefaultVulkanDeviceAllocatorCreateInfo& createInfo);

    VulkanDeviceMemory AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment) override;
    [[nodiscard]] vk::DeviceSize GetMaxAllocationSize() const override;
    void FreeMemory(VulkanDeviceMemory& memory) override;
    void FreeAllMemory() override;

private:
    VulkanDevice* m_vulkanDevice;
    uint32_t m_memoryTypeIndex;
    const VulkanMemoryHeap& m_vulkanMemoryHeap;
    std::vector<VulkanDeviceMemoryPagesAllocator> m_memoryPool;

    void InitializeMemoryPool(bool releaseEmptyPages);
};