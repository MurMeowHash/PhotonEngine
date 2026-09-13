#pragma once

#include "IVulkanDedicatedDeviceAllocator.h"
#include "VulkanMemoryPage.h"

class DefaultVulkanDedicatedDeviceAllocator : public IVulkanDedicatedDeviceAllocator {
public:
    DefaultVulkanDedicatedDeviceAllocator(vk::raii::Device& device, uint32_t memoryTypeIndex);

    VulkanDeviceMemory AllocateImageMemory(const vk::raii::Image& image) override;
    VulkanDeviceMemory AllocateBufferMemory(const vk::raii::Buffer& buffer) override;
    void FreeMemory(VulkanDeviceMemory &memory) override;
    void FreeAllMemory() override;

private:
    vk::raii::Device& m_device;
    uint32_t m_memoryTypeIndex;
    std::vector<VulkanMemoryPage> m_memoryPool;

    VulkanDeviceMemory TryAddPageForMemory(VulkanDeviceMemory&& memory);
};
