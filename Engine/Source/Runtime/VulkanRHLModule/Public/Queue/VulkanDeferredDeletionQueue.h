#pragma once

#include <StaticDelegate.h>
#include <vector>
#include <deque>
#include "FactoryGlobals.h"
#include "CoreGlobals.h"
#include <cstdint>

class VulkanDevice;
class VulkanQueue;

struct VulkanDDQEntry {
    StaticDelegate<> m_deleteDelegate;
};

struct VulkanDDQTimelineStamp {
    VulkanQueue* m_queue;
    uint64_t m_stampedTimelineValue;
};

struct VulkanDDQStampedEntry {
    VulkanDDQEntry m_entry;
    std::vector<VulkanDDQTimelineStamp> m_stamps;

    [[nodiscard]] bool AllStampsCompleted() const;
};

struct VulkanDeferredDeletionQueueCreateInfo {
    VulkanDevice* m_vulkanDevice;
};

class VulkanDeferredDeletionQueue {
public:
    static VulkanDeferredDeletionQueue* Create(const VulkanDeferredDeletionQueueCreateInfo& createInfo,
        InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);

    void Enqueue(const VulkanDDQEntry& entry);
    void StampPendingEntries();
    void ProcessRelease();
    void ReleaseAll();
private:
    VulkanDevice* m_vulkanDevice = nullptr;

    std::vector<VulkanDDQEntry> m_pendingReleaseEntries;
    std::deque<VulkanDDQStampedEntry> m_stampedReleaseEntries;
};