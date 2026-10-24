#include "../Public/Queue/VulkanWorkSyncEvent.h"

VulkanWorkSyncEvent::VulkanWorkSyncEvent(const IDelegate<> &syncDelegate)
: m_syncDelegate(syncDelegate) { }

void VulkanWorkSyncEvent::Execute() const {
    m_syncDelegate.Invoke();
}