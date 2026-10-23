#pragma once

#include "IVulkanDeviceMemoryProvider.h"
#include <unordered_map>
#include <vector>
#include "IVulkanSeparateDeviceAllocator.h"
#include "VulkanDeviceAllocatorBundle.h"
#include "VulkanMemoryHeap.h"

class IAllocator;
class VulkanDevice;

struct VulkanAllocatorsCollection {
    std::vector<VulkanDeviceAllocatorBundle> m_allocatorBundles;
};

struct VulkanDeviceMemoryProviderCreateInfo {
    VulkanDevice* m_vulkanDevice;
    VulkanAllocationPresetDefinition m_vulkanAllocationPresetDefinition;
    bool m_releaseEmptyPages;
};

class VulkanDeviceMemoryProvider : public IVulkanDeviceMemoryProvider {
public:
    explicit VulkanDeviceMemoryProvider(const VulkanDeviceMemoryProviderCreateInfo& createInfo);
    ~VulkanDeviceMemoryProvider() override;
    VulkanDeviceMemoryAllocation AllocateMemory(vk::DeviceSize memorySize, vk::DeviceSize alignment, uint32_t memoryTypeIndex,
        VulkanResourceTilingType resourceTiling) override;
    VulkanDeviceMemoryAllocation AllocateBufferMemory(vk::Buffer buffer, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) override;
    VulkanDeviceMemoryAllocation AllocateImageMemory(vk::Image image, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags, VulkanResourceTilingType resourceTiling) override;
    [[nodiscard]] bool TryFindMemoryTypeIndex(uint32_t memoryTypeRequirementsBits, vk::MemoryPropertyFlags requestedMemoryPropertyFlags,
        uint32_t &memoryTypeIndex) override;
    void FreeMemory(VulkanDeviceMemoryAllocation& allocation) override;
    void FreeAllMemory() override;
private:
    static constexpr uint32_t CHUNK_SIZE_FOR_DEVICE_ALLOCATORS = 2048;

    VulkanDevice* m_vulkanDevice;
    IAllocator* m_allocator;
    vk::PhysicalDeviceMemoryProperties m_memoryProperties;
    bool m_releaseEmptyPages;
    std::unordered_map<VulkanResourceTilingType, VulkanAllocatorsCollection> m_vulkanAllocators;
    std::vector<VulkanMemoryHeap> m_vulkanMemoryHeaps;

    [[nodiscard]] VulkanDeviceMemoryAllocation AllocateFromBundle(VulkanDeviceAllocatorBundle& allocatorBundle,
        vk::DeviceSize memorySize, vk::DeviceSize alignment, VulkanResourceTilingType resourceTiling);
    [[nodiscard]] static VulkanDeviceMemoryAllocation MakeAllocation(const VulkanDeviceMemory& memory,
        VulkanResourceTilingType resourceTiling, VulkanDeviceMemoryAllocatorType allocatorType);
    VulkanDeviceAllocatorBundle& GetAllocatorBundle(VulkanResourceTilingType resourceTiling, uint32_t memoryTypeIndex);
    void InitializeMemoryHeaps(const VulkanAllocationPresetDefinition& vulkanAllocationPresetDefinition);
    std::unordered_map<VulkanResourceTilingType, VulkanAllocatorsCollection>::iterator
    InitializeAllocatorsCollection(VulkanResourceTilingType resourceTiling);
    [[nodiscard]] bool ShouldAllocateDedicatedMemory(const vk::MemoryDedicatedRequirements& dedicatedMemoryRequirements,
        DedicatedMemoryRequirementsFlags dedicatedMemoryRequirementsFlags) const;
};
