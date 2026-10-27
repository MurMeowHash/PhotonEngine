#include "../Public/Queue/VulkanWorkSyncEvent.h"

VulkanWorkSyncEvent::VulkanWorkSyncEvent(StaticDelegate<>&& syncDelegate)
: m_syncDelegate(std::move(syncDelegate)) { }

void VulkanWorkSyncEvent::Execute() const {
    m_syncDelegate.Invoke();
}