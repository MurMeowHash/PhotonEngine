#include "../../Public/Queue/VulkanDeferredDeletionQueue.h"
#include "Device/VulkanDevice.h"

bool VulkanDDQStampedEntry::AllStampsCompleted() const {
    return std::ranges::all_of(m_stamps, [](const VulkanDDQTimelineStamp& stamp) {
        return stamp.m_queue->GetCurrentTimelineValue() >= stamp.m_stampedTimelineValue;
    });
}

VulkanDeferredDeletionQueue* VulkanDeferredDeletionQueue::Create(const VulkanDeferredDeletionQueueCreateInfo &createInfo,
                                                                 InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanDeferredDeletionQueue* instance = Photon::AllocateObject<VulkanDeferredDeletionQueue>(inOutCreateParams);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

void VulkanDeferredDeletionQueue::Enqueue(const VulkanDDQEntry &entry) {
    m_pendingReleaseEntries.emplace_back(entry);
}

void VulkanDeferredDeletionQueue::StampPendingEntries() {
    const std::set<VulkanQueue*>& operatingQueues = m_vulkanDevice->GetOperatingQueues();
    for (VulkanDDQEntry& pendingEntry : m_pendingReleaseEntries) {
        std::vector<VulkanDDQTimelineStamp> ddqTimelineStamps;
        ddqTimelineStamps.reserve(operatingQueues.size());

        for (VulkanQueue* queue : operatingQueues)
            ddqTimelineStamps.emplace_back(queue, queue->GetLastWorkTimelineValue());

        m_stampedReleaseEntries.emplace_back(pendingEntry, std::move(ddqTimelineStamps));
    }

    m_pendingReleaseEntries.clear();
}

void VulkanDeferredDeletionQueue::ProcessRelease() {
    if (m_stampedReleaseEntries.empty())
        return;

    bool entryReleased{};

    do {
        VulkanDDQStampedEntry& stampedEntry = m_stampedReleaseEntries.front();
        if (!stampedEntry.AllStampsCompleted())
            entryReleased = false;
        else {
            stampedEntry.m_entry.m_deleteDelegate.Invoke();
            m_stampedReleaseEntries.pop_front();
            entryReleased = true;
        }
    } while (entryReleased && !m_stampedReleaseEntries.empty());
}

void VulkanDeferredDeletionQueue::ReleaseAll() {
    m_vulkanDevice->WaitIdle();

    for (VulkanDDQEntry& entry : m_pendingReleaseEntries)
        entry.m_deleteDelegate.Invoke();

    m_pendingReleaseEntries.clear();

    while (!m_stampedReleaseEntries.empty()) {
        m_stampedReleaseEntries.front().m_entry.m_deleteDelegate.Invoke();
        m_stampedReleaseEntries.pop_front();
    }
}