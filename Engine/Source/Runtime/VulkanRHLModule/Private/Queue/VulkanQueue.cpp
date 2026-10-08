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

    delete m_workContextPool;
    delete m_workBatchesTimelineSemaphore;
}

VulkanQueue* VulkanQueue::Create(const VulkanQueueCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VulkanQueue* instance = Photon::AllocateObject<VulkanQueue>(inOutCreateParams);
    instance->m_handle = createInfo.m_vulkanDevice->GetHandle().getQueue(createInfo.m_queueFamilyIndex, createInfo.m_queueIndex);
    instance->m_vulkanDevice = createInfo.m_vulkanDevice;
    instance->m_queueFamilyIndex = createInfo.m_queueFamilyIndex;
    Photon::Result workContextPoolResult = instance->CreateWorkContextPool();
    if (workContextPoolResult != Photon::Result::Success) {
        Photon::PushResult(workContextPoolResult, inOutCreateParams);
        return instance;
    }

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
    m_commandBufferPools[commandBufferPool->GetCommandBufferType()][commandBufferPool->GetCommandBufferLifetime()].emplace(commandBufferPool);
}

Photon::Result VulkanQueue::SubmitWorkBatches(VulkanWorkContext* vulkanWorkContext) {
    std::vector<VulkanWorkBatch*> workBatches = vulkanWorkContext->GetAllWorkBatches();

    if (workBatches.empty()) {
        m_pendingInterruptContexts.emplace_back(vulkanWorkContext);
        return Photon::Result::Success;
    }

    std::vector<vk::SubmitInfo2> submitInfos;
    submitInfos.reserve(workBatches.size());
    for (VulkanWorkBatch* workBatch : workBatches) {
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
    m_pendingInterruptContexts.emplace_back(vulkanWorkContext);
    return submitResult == vk::Result::eSuccess ? Photon::Result::Success : Photon::Result::UnknownFailure;
}

void VulkanQueue::ProcessInterruptQueue() {
    if (m_pendingInterruptContexts.empty())
        return;

    std::vector<size_t> contextsIndicesToDispose;
    contextsIndicesToDispose.reserve(m_pendingInterruptContexts.size());

    for (size_t i = 0; i < m_pendingInterruptContexts.size(); i++) {
        std::vector<VulkanWorkBatch*> workBatches = m_pendingInterruptContexts[i]->GetAllWorkBatches();
        for (size_t batchIndex = 0; batchIndex < workBatches.size(); batchIndex++) {
            uint64_t workBatchSemaphoreTimelineValue;
            if (m_workBatchesTimelineSemaphore->TryGetCurrentTimelineValue(workBatchSemaphoreTimelineValue) != Photon::Result::Success)
                continue;

            if (workBatches[batchIndex]->m_timelineSemaphoreFinishedValue <= workBatchSemaphoreTimelineValue)
                m_pendingInterruptContexts[i]->DisposePackedBatch(batchIndex);
        }

        if (!m_pendingInterruptContexts[i]->HasAnyBatches())
            contextsIndicesToDispose.emplace_back(i);
    }

    for (size_t contextIndex : contextsIndicesToDispose) {
        m_workContextPool->ReturnObject(m_pendingInterruptContexts[contextIndex]);
        std::swap(m_pendingInterruptContexts[contextIndex], m_pendingInterruptContexts[m_pendingInterruptContexts.size() - 1]);
        m_pendingInterruptContexts.pop_back();
    }
}

VulkanWorkContextPool* VulkanQueue::GetWorkContextPool() const {
    return m_workContextPool;
}

Photon::Result VulkanQueue::CreateWorkContextPool() {
    VulkanWorkContextPoolCreateInfo createInfo{};
    createInfo.m_vulkanQueue = this;
    InOutCreateParams<Photon::Result> inOutCreateParams{};
    VulkanWorkContextPool* workContextPool = VulkanWorkContextPool::Create(createInfo, &inOutCreateParams);
    if (inOutCreateParams.m_result != Photon::Result::Success)
        return inOutCreateParams.m_result;

    m_workContextPool = workContextPool;
    return Photon::Result::Success;
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