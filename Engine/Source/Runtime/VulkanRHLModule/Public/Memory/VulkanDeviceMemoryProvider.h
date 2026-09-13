#pragma once

#include "IVulkanDeviceMemoryProvider.h"
#include <unordered_map>
#include <vector>
#include "IVulkanDedicatedDeviceAllocator.h"
#include "VulkanDeviceAllocatorBundle.h"
#include "VulkanMemoryHeap.h"

class IAllocator;

struct VulkanAllocatorsCollection {
    std::vector<VulkanDeviceAllocatorBundle> m_allocatorBundles;
};

class VulkanDeviceMemoryProvider : public IVulkanDeviceMemoryProvider {
public:
    static const std::string MEMORY_ALLOCATION_TILING_ATTRIBUTE;

    VulkanDeviceMemoryProvider(vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice,
                               const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition);
    ~VulkanDeviceMemoryProvider() override;
    VulkanDeviceMemory AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment, uint32_t memoryTypeIndex,
        VulkanResourceTilingType resourceTiling) override;
    VulkanDeviceMemory AllocateBufferMemory(const vk::raii::Buffer &buffer, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) override;
    VulkanDeviceMemory AllocateImageMemory(const vk::raii::Image &image, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags, VulkanResourceTilingType resourceTiling) override;
    [[nodiscard]] bool TryFindMemoryTypeIndex(uint32_t memoryTypeRequirementsBits, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        uint32_t &memoryTypeIndex) override;
    void FreeMemory(VulkanDeviceMemory &deviceMemory) override;
    void FreeAllMemory() override;
private:
    static constexpr uint32_t CHUNK_SIZE_FOR_DEVICE_ALLOCATORS = 2048;

    vk::raii::Device& m_device;
    vk::raii::PhysicalDevice& m_physicalDevice;
    vk::PhysicalDeviceMemoryProperties m_memoryProperties;
    IAllocator* m_allocator;
    std::unordered_map<VulkanResourceTilingType, VulkanAllocatorsCollection> m_vulkanAllocators;
    std::vector<VulkanMemoryHeap> m_vulkanMemoryHeaps;

    VulkanDeviceAllocatorBundle& GetAllocatorBundle(VulkanResourceTilingType resourceTiling, uint32_t memoryTypeIndex);

    void InitializeMemoryProperties();
    void InitializeMemoryHeaps(const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition);
    std::unordered_map<VulkanResourceTilingType, VulkanAllocatorsCollection>::iterator
    InitializeAllocatorsCollection(VulkanResourceTilingType resourceTiling);
    [[nodiscard]] bool ShouldAllocateDedicatedMemory(const vk::MemoryDedicatedRequirements& dedicatedMemoryRequirements,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) const;
};