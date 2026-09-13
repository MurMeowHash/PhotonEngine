#pragma once

#include <set>
#include "VulkanDeviceFeature.h"
#include "Entity/VulkanEntity.h"
#include "Memory/IVulkanDeviceMemoryProvider.h"
#include "Memory/VulkanMemoryProviderType.h"
#include "Queue/VulkanQueue.h"

struct VulkanDeviceCreateInfo {
    vk::raii::PhysicalDevice& m_physicalDevice;
    vk::QueueFlags m_requestedQueues;
    std::vector<VulkanExtension> m_requestedExtensions;
    std::vector<VulkanDeviceFeature> m_requestedFeatures;
    VulkanMemoryProviderType m_memoryProviderType = VulkanMemoryProviderType::Default;
};

struct VulkanDeviceFeaturesAssembleData {
    vk::PhysicalDeviceFeatures2 m_featuresChain;
    std::unordered_map<VulkanDeviceFeatureType, VulkanDeviceFeature> m_queryResult;

    vk::PhysicalDeviceVulkan11Features m_features11;
    vk::PhysicalDeviceVulkan12Features m_features12;
    vk::PhysicalDeviceVulkan13Features m_features13;
    vk::PhysicalDeviceVulkan14Features m_features14;
};

struct VulkanFeatureNative {
    vk::Bool32& m_featureEnabledRef;
    vk::Bool32 m_isSupported;
};

class IAllocator;

struct QueueInitializeInfo {
    std::vector<vk::DeviceQueueCreateInfo> m_vulkanCreateInfo;
    IAllocator* m_prioritiesAllocator;
    std::unordered_map<uint32_t, uint32_t> m_queueFamilyRequestProperties;
    std::unordered_map<vk::QueueFlagBits, uint32_t> m_queueFamilyRequestInfo;
};

class VulkanDevice : VulkanEntity {
public:
    ~VulkanDevice() override;
    [[nodiscard]] bool TryGetAvailableExtensions(std::vector<vk::ExtensionProperties> &availableExtensions) const override;
public:
    [[nodiscard]] bool Create(const VulkanDeviceCreateInfo& createInfo);
    [[nodiscard]] vk::raii::Device& GetHandle();
    [[nodiscard]] std::set<VulkanQueue*> GetOperatingQueues() const;
    [[nodiscard]] bool TryGetQueue(vk::QueueFlagBits queueFlagBits, VulkanQueue* vulkanQueue) const;
    [[nodiscard]] IVulkanDeviceMemoryProvider* GetMemoryProvider() const;
private:
    vk::raii::PhysicalDevice m_physicalDevice = nullptr;
    vk::raii::Device m_handle = nullptr;

    std::unordered_map<vk::QueueFlagBits, VulkanQueue*> m_deviceQueues;
    std::unordered_map<VulkanDeviceFeatureType, VulkanDeviceFeature> m_features;

    IVulkanDeviceMemoryProvider* m_memoryProvider = nullptr;

    QueueInitializeInfo InitializeDeviceQueues(vk::QueueFlags requestedQueues);
    void ObtainQueues(std::unordered_map<uint32_t, uint32_t>&& queueFamilyRequestProperties,
        std::unordered_map<vk::QueueFlagBits, uint32_t>&& queueFamilyRequestInfo);

    [[nodiscard]] VulkanDeviceFeaturesAssembleData AssembleDeviceFeatures(const std::vector<VulkanDeviceFeature>& requestedFeatures);
    VulkanFeatureNative ResolveFeature(VulkanDeviceFeatureType featureType,
        const vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features,
                vk::PhysicalDeviceVulkan14Features>& deviceFeaturesInfo, uint32_t deviceApiVersion,
                VulkanDeviceFeaturesAssembleData& assembleData);
    void InitializeMemoryProvider(VulkanMemoryProviderType memoryProviderType);
};