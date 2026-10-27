#pragma once

#include <StaticDelegate.h>

class VulkanWorkSyncEvent {
public:
    explicit VulkanWorkSyncEvent(StaticDelegate<>&& syncDelegate);
    void Execute() const;
private:
    StaticDelegate<> m_syncDelegate;
};