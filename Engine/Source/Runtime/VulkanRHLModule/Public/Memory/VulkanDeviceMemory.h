#pragma once

#include <vulkan/vulkan.hpp>
#include "Memory.h"
#include "CoreGlobals.h"

class VulkanDevice;

struct VulkanDeviceMemoryInitializeInfo {
    VulkanDevice* m_vulkanDevice;
    vk::DeviceMemory m_handle;
    uint32_t m_memoryTypeIndex;
    bool m_dedicated;
    MemoryInitializeInfo<vk::DeviceSize> m_memInfo;
    void* m_mappedRange;
    uint32_t m_allocationTier;
};

class VulkanDeviceMemory final : public Memory<vk::DeviceSize> {
public:
    void InvalidateMemory() override;
public:
    explicit VulkanDeviceMemory();
    VulkanDeviceMemory(const VulkanDeviceMemoryInitializeInfo& initializeInfo);
    void Initialize(const VulkanDeviceMemoryInitializeInfo& initializeInfo);

    [[nodiscard]] uint32_t GetMemoryTypeIndex() const;
    [[nodiscard]] uint32_t GetAllocationTier() const;
    [[nodiscard]] vk::DeviceMemory GetHandle() const;
    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] bool IsDedicated() const;
    [[nodiscard]] bool IsMemoryMappable() const;
    [[nodiscard]] void* GetMappedRange() const;
    [[nodiscard]] void* GetWriteRange() const;
    [[nodiscard]] bool IsMemoryMapped() const;
    [[nodiscard]] bool TryGetMappedRange(void*& mappedRange) const;

    Photon::Result MapMemory();
    void UnMapMemory();

private:
    VulkanDevice* m_vulkanDevice;
    vk::DeviceMemory m_handle;
    uint32_t m_memoryTypeIndex;
    bool m_dedicated;
    void* m_mappedRange = nullptr;
    uint32_t m_allocationTier{};
};