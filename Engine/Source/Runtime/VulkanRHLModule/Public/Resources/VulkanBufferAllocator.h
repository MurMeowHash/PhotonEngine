#pragma once

#include "VulkanRHLResourceDefinitions.h"
#include <unordered_map>
#include "VulkanBufferPage.h"
#include "VulkanUtils.h"
#include "Configurations/VulkanResourceConfiguration.h"

class VulkanDevice;

struct VulkanBufferSliceAllocation {
    VulkanBufferSlice m_bufferSlice;
    bool m_useSingleBuffer;
    vk::BufferUsageFlags m_bufferUsage;
    VulkanRHLBufferUpdateFrequency m_updateFrequency;

    [[nodiscard]] bool IsValid() const;
    void Invalidate();
};

class VulkanBufferSubAllocator {
public:
    virtual ~VulkanBufferSubAllocator() = default;
    VulkanBufferSubAllocator(VulkanDevice* vulkanDevice, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize pageSize);

    virtual VulkanBufferSliceAllocation AllocateBuffer(vk::DeviceSize bufferSize, vk::BufferUsageFlags bufferUsage) = 0;
    virtual void FreeBuffer(VulkanBufferSliceAllocation& bufferSlice) = 0;

protected:
    VulkanDevice* m_vulkanDevice;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    vk::DeviceSize m_pageSize;

    [[nodiscard]] vk::DeviceSize GetBufferAlignment(vk::BufferUsageFlags bufferUsage) const;
};

class GeneralVulkanBufferSubAllocator : public VulkanBufferSubAllocator {
public:
    VulkanBufferSliceAllocation AllocateBuffer(vk::DeviceSize bufferSize, vk::BufferUsageFlags bufferUsage) override;
    void FreeBuffer(VulkanBufferSliceAllocation &bufferSlice) override;

public:
    GeneralVulkanBufferSubAllocator(VulkanDevice* vulkanDevice, vk::MemoryPropertyFlags memoryPropertyFlags, vk::DeviceSize pageSize);

private:
    std::unordered_map<vk::BufferUsageFlags, VulkanBufferPagesAllocator, Photon::Vulkan::FlagsHasher<vk::BufferUsageFlagBits>> m_pooledBuffers;
    VulkanBufferPagesAllocatorSeparate m_singleBuffers;
};

struct VulkanBufferAllocatorCreateInfo {
    VulkanDevice* m_vulkanDevice;
};

class VulkanBufferAllocator {
public:
    ~VulkanBufferAllocator();
    static VulkanBufferAllocator* Create(const VulkanBufferAllocatorCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    VulkanBufferSliceAllocation AllocateBuffer(vk::DeviceSize bufferSize, VulkanRHLBufferUpdateFrequency updateFrequency, VulkanRHLBufferUsage bufferUsage);
    void FreeBuffer(VulkanBufferSliceAllocation& buffer);
private:
    VulkanDevice* m_vulkanDevice = nullptr;
    std::unordered_map<VulkanRHLBufferUpdateFrequency, VulkanBufferSubAllocator*> m_bufferSubAllocators;

    template<std::derived_from<VulkanBufferSubAllocator> TBufferSubAllocator>
    void AddBufferSubAllocator(VulkanRHLBufferUpdateFrequency updateFrequency) {
        vk::MemoryPropertyFlags memoryPropertyFlags = Photon::Vulkan::ResolveVulkanMemoryFlagsFromUpdateFrequency(updateFrequency);
        vk::DeviceSize pageSize = Photon::Vulkan::BufferPoolAllocationConfiguration::g_bufferPoolMaxSize[updateFrequency];
        m_bufferSubAllocators[updateFrequency] = new TBufferSubAllocator(m_vulkanDevice, memoryPropertyFlags, pageSize);
    }
};