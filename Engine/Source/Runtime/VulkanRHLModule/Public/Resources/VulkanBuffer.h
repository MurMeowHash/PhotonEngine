#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "Memory.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "VulkanResource.h"
#include "Memory/IVulkanDeviceMemoryProvider.h"
#include "VulkanResourceTransition.h"

class VulkanDevice;
enum class DedicatedMemoryRequirementsFlags : uint32_t;
class VulkanBufferPage;

struct VulkanBufferCreateInfo {
    VulkanDevice* m_vulkanDevice;
    vk::DeviceSize m_size;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    DedicatedMemoryRequirementsFlags m_dedicatedRequirements;
};

class VulkanBuffer : public VulkanResource {
public:
    [[nodiscard]] bool IsDedicated() const override;
    void CommitPendingTransitionScope() override;

public:
    ~VulkanBuffer() override;
    static VulkanBuffer* Create(const VulkanBufferCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    [[nodiscard]] vk::Buffer GetHandle() const;
    void TrackLastTransitionScope(const VulkanPipelineBufferTransitionScope& transitionScope);
    Photon::Result WriteData(void* data, vk::DeviceSize dataSize);
    void* GetDataWriteDest() const;
private:
    vk::raii::Buffer m_handle = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;
    VulkanDeviceMemoryAllocation m_allocation;

    VulkanPipelineBufferTransitionScope m_pendingTransitionScope{};

    [[nodiscard]] Photon::Result AllocateBufferMemory(vk::MemoryPropertyFlags memoryPropertyFlags, DedicatedMemoryRequirementsFlags dedicatedRequirements);
};

struct VulkanBufferSliceInitializeInfo {
    VulkanBuffer* m_buffer;
    MemoryInitializeInfo<vk::DeviceSize> m_memInfo;
};

class VulkanBufferSlice final : public Memory<vk::DeviceSize> {
public:
    void InvalidateMemory() override;
public:
    explicit VulkanBufferSlice();
    explicit VulkanBufferSlice(const VulkanBufferSliceInitializeInfo& initializeInfo);
    void Initialize(const VulkanBufferSliceInitializeInfo& initializeInfo);
    [[nodiscard]] VulkanBuffer* GetBuffer() const;
    [[nodiscard]] bool IsValid() const;
private:
    VulkanBuffer* m_buffer = nullptr;
};