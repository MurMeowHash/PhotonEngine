#include "../../Public/Queue/VulkanQueue.h"
#include "Device/VulkanDevice.h"
#include "CommandBuffers/VulkanCommandBufferPool.h"
#include "Configurations/VulkanFrameConfiguration.h"
#include "Queue/VulkanWorkBatch.h"
#include "Queue/VulkanWorkContext.h"
#include "Synchronization/VulkanTimelineSemaphore.h"

VulkanQueue::~VulkanQueue() {
    if (GetWorkBatchesLeft() != 0) {
        vk::Result waitResult = m_handle.waitIdle();
        ProcessInterruptQueue();
    }

    for (auto& commandBufferPool : m_commandBufferPools) {
        for (auto& pool : commandBufferPool.second) {
            while (!pool.second.empty()) {
                VulkanCommandBufferPool* vulkanCommandPool = pool.second.front();
                delete vulkanCommandPool;
                pool.second.pop();
            }
        }
    }

    delete m_workBatchesTimelineSemaphore;
}

VulkanQueue* VulkanQueue::Create(const VulkanQueueCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanQueue* instance = Photon::AllocateObject<VulkanQueue>(inOutCreateParams);
    instance->m_handle = createInfo.m_vulkanDevice->GetHandle().getQueue(createInfo.m_queueFamilyIndex, createInfo.m_queueIndex);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    instance->m_queueFamilyIndex = createInfo.m_queueFamilyIndex;
    Photon::Result timelineSemaphoreResult = instance->CreateTimelineSemaphore();
    Photon::PushResult(timelineSemaphoreResult, inOutCreateParams);
    return instance;
}

uint32_t VulkanQueue::GetQueueFamilyIndex() const {
    return m_queueFamilyIndex;
}

VulkanCommandBufferPool* VulkanQueue::AcquireCommandBufferPool(VulkanCommandBufferType commandBufferType,
    VulkanCommandBufferLifetime commandBufferLifetime, Photon::Result& acquireResult) {
    auto commandBufferPoolsIterator = m_commandBufferPools.find(commandBufferType);
    if (commandBufferPoolsIterator != m_commandBufferPools.end()) {
        auto commandPoolIterator = commandBufferPoolsIterator->second.find(commandBufferLifetime);
        if (commandPoolIterator != commandBufferPoolsIterator->second.end() && !commandPoolIterator->second.empty()) {
            VulkanCommandBufferPool* commandBufferPool = commandPoolIterator->second.front();
            commandPoolIterator->second.pop();
            acquireResult = Photon::Result::Success;
            return commandBufferPool;
        }
    }

    VulkanCommandBufferPoolCreateInfo createInfo(
        m_vulkanDevice,
        commandBufferLifetime,
        VulkanCommandBufferCreateFlags::AllowDedicatedReset,
        commandBufferType,
        this,
        0);

    InOutCreateParams<Photon::Result> inOutCreateParams{};
    VulkanCommandBufferPool* commandBufferPool = VulkanCommandBufferPool::Create(createInfo, &inOutCreateParams);
    acquireResult = inOutCreateParams.m_result;
    return commandBufferPool;
}

void VulkanQueue::ReturnCommandBufferPool(VulkanCommandBufferPool *commandBufferPool) {
    if (commandBufferPool == nullptr)
        return;

    m_commandBufferPools[commandBufferPool->GetCommandBufferType()][commandBufferPool->GetCommandBufferLifetime()].emplace(commandBufferPool);
}

Photon::Result VulkanQueue::SubmitWorkBatches(VulkanWorkSubmitInfo&& workSubmitInfo) {
    if (workSubmitInfo.m_workBatches.empty())
        return Photon::Result::Success;

    const size_t batchCount = workSubmitInfo.m_workBatches.size();
    std::vector<vk::SubmitInfo2> submitInfos(batchCount);
    std::vector<std::vector<vk::SemaphoreSubmitInfo>> waitSemaphoresInfo(batchCount);
    std::vector<std::vector<vk::CommandBufferSubmitInfo>> commandBuffersInfo(batchCount);
    std::vector<std::vector<vk::SemaphoreSubmitInfo>> signalSemaphoresInfo(batchCount);

    for (size_t batch = 0; batch < batchCount; batch++) {
        VulkanWorkBatch* workBatch = workSubmitInfo.m_workBatches[batch];
        assert(workBatch->m_waitSemaphores.size() == workBatch->m_waitSemaphoresFlags.size());

        const size_t waitSemaCount = workBatch->m_waitSemaphores.size();
        waitSemaphoresInfo[batch].reserve(waitSemaCount);
        for (size_t i = 0; i < waitSemaCount; i++)
            waitSemaphoresInfo[batch].emplace_back(workBatch->m_waitSemaphores[i]->ScheduleAcquireSubmit(workBatch->m_waitSemaphoresFlags[i]));

        commandBuffersInfo[batch].reserve(workBatch->m_commandBuffers.size());
        for (VulkanCommandBuffer* commandBuffer: workBatch->m_commandBuffers) {
            vk::CommandBufferSubmitInfo cmdBufferSubmitInfo{};
            cmdBufferSubmitInfo.commandBuffer = commandBuffer->GetHandle();
            cmdBufferSubmitInfo.deviceMask = 0;
            commandBuffersInfo[batch].emplace_back(cmdBufferSubmitInfo);
        }

        signalSemaphoresInfo[batch].reserve(workBatch->m_signalSemaphores.size() + 1);
        for (std::shared_ptr<VulkanSemaphore>& signalSemaphore : workBatch->m_signalSemaphores)
            signalSemaphoresInfo[batch].emplace_back(signalSemaphore->ScheduleReleaseSubmit());

        signalSemaphoresInfo[batch].emplace_back(m_workBatchesTimelineSemaphore->ScheduleReleaseSubmit());
        workBatch->m_timelineSemaphoreFinishedValue = m_workBatchesTimelineSemaphore->GetCurrentScheduledValue();

        vk::SubmitInfo2& submitInfo = submitInfos[batch];
        submitInfo.waitSemaphoreInfoCount = waitSemaphoresInfo[batch].size();
        submitInfo.pWaitSemaphoreInfos = waitSemaphoresInfo[batch].data();
        submitInfo.commandBufferInfoCount = commandBuffersInfo[batch].size();
        submitInfo.pCommandBufferInfos = commandBuffersInfo[batch].data();
        submitInfo.signalSemaphoreInfoCount = signalSemaphoresInfo[batch].size();
        submitInfo.pSignalSemaphoreInfos = signalSemaphoresInfo[batch].data();
    }

    vk::Result submitResult = m_handle.submit2(submitInfos);
    m_pendingInterruptQueue.emplace_back(std::move(workSubmitInfo));
    return submitResult == vk::Result::eSuccess ? Photon::Result::Success : Photon::Result::UnknownFailure;
}

vk::Queue VulkanQueue::GetHandle() const {
    return m_handle;
}

void VulkanQueue::ProcessInterruptQueue() {
    if (m_pendingInterruptQueue.empty())
        return;

    uint64_t completedTimelineValue;
    if (m_workBatchesTimelineSemaphore->TryGetCurrentTimelineValue(completedTimelineValue) != Photon::Result::Success)
        return;

    while (!m_pendingInterruptQueue.empty()) {
        VulkanWorkSubmitInfo& workSubmitInfo = m_pendingInterruptQueue.front();
        for (size_t batchIndex = 0; batchIndex < workSubmitInfo.m_workBatches.size(); batchIndex++) {
            VulkanWorkBatch* workBatch = workSubmitInfo.m_workBatches[batchIndex];
            if (workBatch->m_batchStatus != VulkanWorkBatchStatus::Finished
                && workBatch->m_timelineSemaphoreFinishedValue <= completedTimelineValue)
                workSubmitInfo.FinishBatch(batchIndex);
        }

        if (!workSubmitInfo.IsAllBatchesFinished())
            break;

        workSubmitInfo.Dispose(this, m_vulkanDevice->GetWorkAllocatorPool());
        m_pendingInterruptQueue.pop_front();
    }
}

Photon::Result VulkanQueue::WaitUntilWorkBatchFinished(uint32_t batchIndex) const {
    if (batchIndex >= m_pendingInterruptQueue.size())
        return Photon::Result::Success;

    uint64_t batchFinishTimelineValue = m_pendingInterruptQueue[batchIndex].GetBatchMaxTimelineValue(VulkanWorkBatchStatus::Packed);
    return m_workBatchesTimelineSemaphore->WaitForValue(batchFinishTimelineValue);
}

uint32_t VulkanQueue::GetWorkBatchesLeft() const {
    return m_pendingInterruptQueue.size();
}

Photon::Result VulkanQueue::CreateTimelineSemaphore() {
    VulkanTimelineSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.m_vulkanDevice = m_vulkanDevice;
    semaphoreCreateInfo.m_initialValue = 0;
    InOutCreateParams<Photon::Result> semaphoreInOutCreateParams{};
    VulkanTimelineSemaphore* timelineSemaphore = VulkanTimelineSemaphore::Create(semaphoreCreateInfo, &semaphoreInOutCreateParams);
    if (semaphoreInOutCreateParams.m_result != Photon::Result::Success)
        return Photon::Result::UnknownFailure;

    m_workBatchesTimelineSemaphore = timelineSemaphore;
    return Photon::Result::Success;
}
