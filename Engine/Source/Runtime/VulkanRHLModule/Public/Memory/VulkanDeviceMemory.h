#pragma once

#include <vulkan/vulkan.hpp>
#include "RuntimeAttributes.h"
#include "Memory.h"
#include "CoreGlobals.h"

struct VulkanDeviceMemoryInitializeInfo {
    vk::Device m_device;
    vk::DeviceMemory m_handle;
    uint32_t m_memoryTypeIndex;
    bool m_dedicated;
    MemoryInitializeInfo<vk::DeviceSize> m_memInfo;
};

class VulkanDeviceMemory final : public Memory<vk::DeviceSize> {
public:
    void InvalidateMemory() override;
public:
    explicit VulkanDeviceMemory();
    VulkanDeviceMemory(const VulkanDeviceMemoryInitializeInfo& initializeInfo);
    void Initialize(const VulkanDeviceMemoryInitializeInfo& initializeInfo);

    [[nodiscard]] uint32_t GetMemoryTypeIndex() const;
    [[nodiscard]] vk::DeviceMemory GetHandle() const;
    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] bool IsDedicated() const;
    [[nodiscard]] RuntimeAttributes& GetMemoryAttributes();
    [[nodiscard]] void* GetMappedRange() const;
    [[nodiscard]] bool TryGetMappedRange(void*& mappedRange) const;

    [[nodiscard]] Photon::Result MapMemory();
    void UnMapMemory();

private:
    vk::Device m_device;
    vk::DeviceMemory m_handle;
    uint32_t m_memoryTypeIndex;
    bool m_dedicated;
    RuntimeAttributes m_memoryAttributes;
    void* m_mappedRange = nullptr;
};