#include "../../Public/Queue/VulkanQueue.h"
#include "Device/VulkanDevice.h"
#include "CommandBuffers/VulkanCommandBufferPool.h"
#include "Queue/VulkanWorkBatch.h"
#include "Queue/VulkanWorkContext.h"
#include "Synchronization/VulkanTimelineSemaphore.h"

VulkanQueue::~VulkanQueue() {
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
    if (workSubmitInfo.m_workBatches.empty()) {
        m_pendingInterruptQueue.emplace_back(std::move(workSubmitInfo));
        return Photon::Result::Success;
    }

    std::vector<vk::SubmitInfo2> submitInfos;
    submitInfos.reserve(workSubmitInfo.m_workBatches.size());
    for (VulkanWorkBatch* workBatch : workSubmitInfo.m_workBatches) {
        assert(workBatch->m_waitSemaphores.size() == workBatch->m_waitSemaphoresFlags.size());

        std::vector<vk::SemaphoreSubmitInfo> waitSemaphoresInfo(workBatch->m_waitSemaphores.size());
        for (size_t i = 0; i < waitSemaphoresInfo.size(); i++)
            waitSemaphoresInfo[i] = workBatch->m_waitSemaphores[i]->ScheduleAcquireSubmit(workBatch->m_waitSemaphoresFlags[i]);

        std::vector<vk::CommandBufferSubmitInfo> commandBuffersInfo;
        commandBuffersInfo.reserve(workBatch->m_commandBuffers.size());
        for (VulkanCommandBuffer* commandBuffer: workBatch->m_commandBuffers) {
            vk::CommandBufferSubmitInfo cmdBufferSubmitInfo{};
            cmdBufferSubmitInfo.commandBuffer = commandBuffer->GetHandle();
            cmdBufferSubmitInfo.deviceMask = 0;
            commandBuffersInfo.emplace_back(cmdBufferSubmitInfo);
        }

        std::vector<vk::SemaphoreSubmitInfo> signalSemaphoresInfo;
        signalSemaphoresInfo.reserve(workBatch->m_signalSemaphores.size() + 1);
        for (VulkanSemaphore* signalSemaphore: workBatch->m_signalSemaphores)
            signalSemaphoresInfo.emplace_back(signalSemaphore->ScheduleReleaseSubmit());

        signalSemaphoresInfo.emplace_back(m_workBatchesTimelineSemaphore->ScheduleReleaseSubmit());
        workBatch->m_timelineSemaphoreFinishedValue = m_workBatchesTimelineSemaphore->GetCurrentScheduledValue();

        vk::SubmitInfo2 submitInfo{};
        submitInfo.waitSemaphoreInfoCount = waitSemaphoresInfo.size();
        submitInfo.pWaitSemaphoreInfos = waitSemaphoresInfo.data();
        submitInfo.commandBufferInfoCount = commandBuffersInfo.size();
        submitInfo.pCommandBufferInfos = commandBuffersInfo.data();
        submitInfo.signalSemaphoreInfoCount = signalSemaphoresInfo.size();
        submitInfo.pSignalSemaphoreInfos = signalSemaphoresInfo.data();

        submitInfos.emplace_back(submitInfo);
    }

    vk::Result submitResult = m_handle.submit2(submitInfos);
    m_pendingInterruptQueue.emplace_back(std::move(workSubmitInfo));
    return submitResult == vk::Result::eSuccess ? Photon::Result::Success : Photon::Result::UnknownFailure;
}

void VulkanQueue::ProcessInterruptQueue() {
    if (m_pendingInterruptQueue.empty())
        return;

    std::vector<size_t> workSubmitInfosToDispose;
    workSubmitInfosToDispose.reserve(m_pendingInterruptQueue.size());

    for (size_t i = 0; i < m_pendingInterruptQueue.size(); i++) {
        VulkanWorkSubmitInfo& workSubmitInfo = m_pendingInterruptQueue[i];
        for (size_t batchIndex = 0; batchIndex < workSubmitInfo.m_workBatches.size(); batchIndex++) {
            if (workSubmitInfo.m_workBatches[batchIndex]->m_batchStatus == VulkanWorkBatchStatus::Finished)
                continue;

            uint64_t workBatchSemaphoreTimelineValue;
            if (m_workBatchesTimelineSemaphore->TryGetCurrentTimelineValue(workBatchSemaphoreTimelineValue) != Photon::Result::Success)
                continue;

            if (workSubmitInfo.m_workBatches[batchIndex]->m_timelineSemaphoreFinishedValue <= workBatchSemaphoreTimelineValue)
                workSubmitInfo.FinishBatch(batchIndex);
        }

        if (workSubmitInfo.IsAllBatchesFinished())
            workSubmitInfosToDispose.emplace_back(i);
    }

    for (size_t workSubmitIndex : workSubmitInfosToDispose) {
        m_pendingInterruptQueue[workSubmitIndex].Dispose(this, m_vulkanDevice->GetWorkAllocatorPool());
        std::swap(m_pendingInterruptQueue[workSubmitIndex], m_pendingInterruptQueue.back());
        m_pendingInterruptQueue.pop_back();
    }
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