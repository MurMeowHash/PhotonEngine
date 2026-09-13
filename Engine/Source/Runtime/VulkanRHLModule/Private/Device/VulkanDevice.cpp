#include "../../Public/Device/VulkanDevice.h"
#include "CoreUtils.h"
#include "LinearAllocator.h"
#include "Memory/VulkanDeviceMemoryProviderFactory.h"
#include "Memory/Configurations/VulkanAllocationConfiguration.h"
#include "Queue/VulkanQueueFactory.h"

using Photon::Core::Flags::operator|;

#define QUERY_FEATURE_SUPPORT_SAFE(featureType, featureName, deviceFeatureInfo, deviceApiVersion, featuresApiVersion)   \
    deviceApiVersion < featuresApiVersion ? vk::False : deviceFeatureInfo.get<featureType>().featureName

#define ADD_FEATURE_TO_CHAIN_SAFE(featureChainEnd, features, deviceApiVersion, featuresApiVersion)                      \
    if(deviceApiVersion >= featuresApiVersion) {                                                                        \
        *featureChainEnd = &features;                                                                                   \
        featureChainEnd = &features.pNext;                                                                              \
    }

VulkanDevice::~VulkanDevice() {
    delete m_memoryProvider;

    for (std::pair<const vk::QueueFlagBits, VulkanQueue *>& queue: m_deviceQueues) {
        delete queue.second;
    }
}

bool VulkanDevice::TryGetAvailableExtensions(std::vector<vk::ExtensionProperties> &availableExtensions) const {
    vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedDeviceExtensions = m_physicalDevice.enumerateDeviceExtensionProperties();
    if (supportedDeviceExtensions.result != vk::Result::eSuccess)
        return false;

    availableExtensions = supportedDeviceExtensions.value;
    return true;
}

bool VulkanDevice::Create(const VulkanDeviceCreateInfo &createInfo) {
    m_physicalDevice = createInfo.m_physicalDevice;
    SetApiVersion(m_physicalDevice.getProperties2().properties.apiVersion);

    QueueInitializeInfo queueInitializationInfo = InitializeDeviceQueues(createInfo.m_requestedQueues);
    ProcessExtensionsRequest(createInfo.m_requestedExtensions);
    VulkanDeviceFeaturesAssembleData featuresAssembleData = AssembleDeviceFeatures(createInfo.m_requestedFeatures);

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = queueInitializationInfo.m_vulkanCreateInfo.size();
    deviceCreateInfo.pQueueCreateInfos = queueInitializationInfo.m_vulkanCreateInfo.data();
    std::vector<const char*> extensions = QueryExtensionNames(VulkanExtensionQueryFilter::Supported | VulkanExtensionQueryFilter::Required);
    deviceCreateInfo.enabledExtensionCount = extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    deviceCreateInfo.pNext = &featuresAssembleData.m_featuresChain;

    vk::ResultValue<vk::raii::Device> deviceWrapper = m_physicalDevice.createDevice(deviceCreateInfo);
    if(deviceWrapper.result != vk::Result::eSuccess)
        return false;

    m_features = featuresAssembleData.m_queryResult;

    m_handle = std::move(deviceWrapper.value);
    ObtainQueues(std::move(queueInitializationInfo.m_queueFamilyRequestProperties), std::move(queueInitializationInfo.m_queueFamilyRequestInfo));
    InitializeMemoryProvider(createInfo.m_memoryProviderType);

    queueInitializationInfo.m_prioritiesAllocator->FreeMemory();
    delete queueInitializationInfo.m_prioritiesAllocator;
    return true;
}

vk::raii::Device & VulkanDevice::GetHandle() {
    return m_handle;
}

std::set<VulkanQueue *> VulkanDevice::GetOperatingQueues() const {
    std::set<VulkanQueue*> queues;
    for (const std::pair<const vk::QueueFlagBits, VulkanQueue*>& queueData : m_deviceQueues) {
        queues.emplace(queueData.second);
    }

    return queues;
}

bool VulkanDevice::TryGetQueue(vk::QueueFlagBits queueFlagBits, VulkanQueue *vulkanQueue) const {
    auto queueIterator = m_deviceQueues.find(queueFlagBits);
    if (queueIterator == m_deviceQueues.end())
        return false;

    vulkanQueue = queueIterator->second;
    return true;
}

IVulkanDeviceMemoryProvider * VulkanDevice::GetMemoryProvider() const {
    return m_memoryProvider;
}

QueueInitializeInfo VulkanDevice::InitializeDeviceQueues(vk::QueueFlags requestedQueues) {
    std::vector<vk::QueueFamilyProperties2> deviceQueueFamilies = m_physicalDevice.getQueueFamilyProperties2();
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    std::unordered_map<uint32_t, uint32_t> queueFamilyRequestProperties;
    std::unordered_map<vk::QueueFlagBits, uint32_t> queueFamilyRequestInfo;
    std::unordered_set<uint32_t> stagingQueueFamilies;
    std::vector<uint32_t> candidateQueueFamilies;
    queueFamilyRequestProperties.reserve(deviceQueueFamilies.size());
    candidateQueueFamilies.reserve(deviceQueueFamilies.size());
    queueFamilyRequestInfo.reserve(deviceQueueFamilies.size());

    uint32_t totalRequestedQueues = 0;

    while(requestedQueues) {
        vk::QueueFlags::MaskType rawQueueFlags = static_cast<vk::QueueFlags::MaskType>(requestedQueues);
        vk::QueueFlags::MaskType lowestRawQueueFlag = rawQueueFlags & (~rawQueueFlags + 1);
        vk::QueueFlagBits inspectedQueueBit = static_cast<vk::QueueFlagBits>(lowestRawQueueFlag);
        requestedQueues &= ~vk::QueueFlags(inspectedQueueBit);

        for(size_t i = 0; i < deviceQueueFamilies.size(); i++) {
            if (deviceQueueFamilies[i].queueFamilyProperties.queueFlags & inspectedQueueBit) {
                std::unordered_map<uint32_t, uint32_t>::iterator queueFamilyRequestIterator = queueFamilyRequestProperties.find(i);
                if (queueFamilyRequestIterator != queueFamilyRequestProperties.end())
                    candidateQueueFamilies.emplace_back(i);
                else {
                    queueFamilyRequestProperties.emplace(i, 1);
                    queueFamilyRequestInfo.emplace(inspectedQueueBit, i);
                    ++totalRequestedQueues;
                    break;
                }
            }
        }

        if (!queueFamilyRequestInfo.contains(inspectedQueueBit) && !candidateQueueFamilies.empty()) {
            ++queueFamilyRequestProperties[candidateQueueFamilies[0]];
            queueFamilyRequestInfo.emplace(inspectedQueueBit, candidateQueueFamilies[0]);
            ++totalRequestedQueues;
        }

        candidateQueueFamilies.clear();
    }

    LinearAllocator* prioritiesAllocator = new LinearAllocator(totalRequestedQueues * sizeof(float));

    QueueInitializeInfo queueInitializeInfo;
    queueInitializeInfo.m_vulkanCreateInfo.reserve(queueFamilyRequestProperties.size());

    for (const std::pair<const uint32_t, uint32_t> &queueFamilyRequest: queueFamilyRequestProperties) {
        uint32_t queueCount = std::min(queueFamilyRequest.second, deviceQueueFamilies[queueFamilyRequest.first].queueFamilyProperties.queueCount);
        float* queuePriorityMemory = static_cast<float*>(prioritiesAllocator->AllocateMemory(queueCount * sizeof(float)));
        std::fill_n(queuePriorityMemory, queueCount, 1.0f);
        vk::DeviceQueueCreateInfo queueInfo{{}, queueFamilyRequest.first, queueCount, queuePriorityMemory};
        queueInitializeInfo.m_vulkanCreateInfo.emplace_back(queueInfo);
        queueFamilyRequestProperties[queueFamilyRequest.first] = queueCount;
    }

    queueInitializeInfo.m_prioritiesAllocator = prioritiesAllocator;
    queueInitializeInfo.m_queueFamilyRequestProperties = std::move(queueFamilyRequestProperties);
    queueInitializeInfo.m_queueFamilyRequestInfo = std::move(queueFamilyRequestInfo);

    return queueInitializeInfo;
}

void VulkanDevice::ObtainQueues(std::unordered_map<uint32_t, uint32_t> &&queueFamilyRequestProperties,
    std::unordered_map<vk::QueueFlagBits, uint32_t> &&queueFamilyRequestInfo) {
    std::vector<std::vector<VulkanQueue*>> createdQueues(m_physicalDevice.getQueueFamilyProperties2().size(), std::vector<VulkanQueue*>());
    m_deviceQueues.clear();

    for(std::pair<const vk::QueueFlagBits, uint32_t> &requestQueueInfo : queueFamilyRequestInfo) {
        if (createdQueues[requestQueueInfo.second].size() >= queueFamilyRequestProperties[requestQueueInfo.second])
            m_deviceQueues.emplace(requestQueueInfo.first, createdQueues[requestQueueInfo.second][0]);
        else {
            VulkanQueueCreateInfo queueCreateInfo;
            queueCreateInfo.m_vulkanDevice = this;
            queueCreateInfo.m_queueFamilyIndex = requestQueueInfo.second;
            queueCreateInfo.m_queueIndex = createdQueues[requestQueueInfo.second].size();
            VulkanQueue* queue = Photon::Vulkan::QueueFactory::CreateQueue(queueCreateInfo);
            m_deviceQueues.emplace(requestQueueInfo.first, queue);
            createdQueues[requestQueueInfo.second].emplace_back(queue);
        }
    }
}

VulkanDeviceFeaturesAssembleData VulkanDevice::AssembleDeviceFeatures(const std::vector<VulkanDeviceFeature> &requestedFeatures) {
    auto deviceFeaturesInfo = m_physicalDevice.getFeatures2<vk::PhysicalDeviceFeatures2,
                                                                            vk::PhysicalDeviceVulkan11Features,
                                                                            vk::PhysicalDeviceVulkan12Features,
                                                                            vk::PhysicalDeviceVulkan13Features,
                                                                            vk::PhysicalDeviceVulkan14Features>();

    uint32_t deviceApiVersion = m_physicalDevice.getProperties().apiVersion;
    VulkanDeviceFeaturesAssembleData featuresAssembleData;

    for (const VulkanDeviceFeature& feature : requestedFeatures) {
        VulkanFeatureNative vulkanFeature = ResolveFeature(feature.GetFeatureType(), deviceFeaturesInfo, deviceApiVersion, featuresAssembleData);
        bool isFeatureEnabled = vulkanFeature.m_isSupported || feature.IsRequired();
        vulkanFeature.m_featureEnabledRef = isFeatureEnabled;
        auto featureIterator = featuresAssembleData.m_queryResult.emplace(feature.GetFeatureType(), feature);
        featureIterator.first->second.MarkEnabled(isFeatureEnabled);
    }
    void** featureChainEnd = &featuresAssembleData.m_featuresChain.pNext;
    ADD_FEATURE_TO_CHAIN_SAFE(featureChainEnd, featuresAssembleData.m_features11, deviceApiVersion, vk::ApiVersion11)
    ADD_FEATURE_TO_CHAIN_SAFE(featureChainEnd, featuresAssembleData.m_features12, deviceApiVersion, vk::ApiVersion12)
    ADD_FEATURE_TO_CHAIN_SAFE(featureChainEnd, featuresAssembleData.m_features13, deviceApiVersion, vk::ApiVersion13)
    ADD_FEATURE_TO_CHAIN_SAFE(featureChainEnd, featuresAssembleData.m_features14, deviceApiVersion, vk::ApiVersion14)

    return featuresAssembleData;
}

VulkanFeatureNative VulkanDevice::ResolveFeature(VulkanDeviceFeatureType featureType,
    const vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::
    PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceVulkan14Features> &
    deviceFeaturesInfo, uint32_t deviceApiVersion, VulkanDeviceFeaturesAssembleData &assembleData) {
    switch (featureType) {
        case VulkanDeviceFeatureType::ShaderDrawParameters:
            return VulkanFeatureNative(assembleData.m_features11.shaderDrawParameters,
                QUERY_FEATURE_SUPPORT_SAFE(vk::PhysicalDeviceVulkan11Features, shaderDrawParameters,
                    deviceFeaturesInfo, deviceApiVersion, vk::ApiVersion11));
        case VulkanDeviceFeatureType::DynamicRendering:
            return VulkanFeatureNative(assembleData.m_features13.dynamicRendering,
                QUERY_FEATURE_SUPPORT_SAFE(vk::PhysicalDeviceVulkan13Features, dynamicRendering,
                    deviceFeaturesInfo, deviceApiVersion, vk::ApiVersion13));
        case VulkanDeviceFeatureType::Synchronization2:
            return VulkanFeatureNative(assembleData.m_features13.synchronization2,
                QUERY_FEATURE_SUPPORT_SAFE(vk::PhysicalDeviceVulkan13Features, synchronization2,
                    deviceFeaturesInfo, deviceApiVersion, vk::ApiVersion13));
        default:
            throw std::invalid_argument(StringFormatter("Unable to resolve feature of type ", static_cast<uint32_t>(featureType)));
    }
}

void VulkanDevice::InitializeMemoryProvider(VulkanMemoryProviderType memoryProviderType) {
    m_memoryProvider = Photon::Vulkan::DeviceMemoryProviderFactory::CreateVulkanDeviceMemoryProvider(memoryProviderType,
        m_handle, m_physicalDevice, Photon::Vulkan::AllocationConfiguration::g_defaultAllocationPreset);
}