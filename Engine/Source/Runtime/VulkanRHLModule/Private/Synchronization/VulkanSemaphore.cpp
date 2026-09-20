#include "../../Public/Synchronization/VulkanSemaphore.h"

const vk::raii::Semaphore& VulkanSemaphore::GetHandle() const {
    return m_handle;
}