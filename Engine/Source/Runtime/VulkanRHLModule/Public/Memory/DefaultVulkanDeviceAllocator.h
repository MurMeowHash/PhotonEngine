#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "IVulkanDeviceAllocator.h"
#include "VulkanMemoryHeap.h"
#include "VulkanMemoryPage.h"

class DefaultVulkanDeviceAllocator : public IVulkanDeviceAllocator {
public:
    DefaultVulkanDeviceAllocator(vk::raii::Device& device, const VulkanMemoryHeap& vulkanMemoryHeap, uint32_t memoryTypeIndex);

    VulkanDeviceMemory AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment) override;
    void FreeMemory(VulkanDeviceMemory& memory) override;
    void FreeAllMemory(bool deallocate) override;

private:
    vk::raii::Device& m_device;
    uint32_t m_memoryTypeIndex;
    const VulkanMemoryHeap& m_vulkanMemoryHeap;
    std::vector<std::vector<VulkanMemoryPage>> m_memoryPool;

    void InitializeMemoryPool();
};