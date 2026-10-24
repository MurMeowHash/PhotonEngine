#pragma once

#include <IDelegate.h>

class VulkanWorkSyncEvent {
public:
    explicit VulkanWorkSyncEvent(const IDelegate<>& syncDelegate);
    void Execute() const;
private:
    const IDelegate<>& m_syncDelegate;
};