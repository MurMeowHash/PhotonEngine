#include "../../Public/Queue/VulkanWorkContext.h"

void VulkanWorkSubmitInfo::FinishBatch(size_t batchIndex) {
    if (batchIndex >= m_workBatches.size())
        return;

    m_workBatches[batchIndex]->m_batchStatus = VulkanWorkBatchStatus::Finished;
    for (VulkanCommandBuffer* cmdBuffer: m_workBatches[batchIndex]->m_commandBuffers)
        m_commandBufferPool->ReturnCommandBuffer(cmdBuffer);

    ++m_finishedBatches;
}

void VulkanWorkSubmitInfo::DisposeBatch(size_t batchIndex) {
    if (batchIndex >= m_workBatches.size())
        return;

    for (VulkanCommandBuffer* cmdBuffer : m_workBatches[batchIndex]->m_commandBuffers)
        m_commandBufferPool->ReturnCommandBuffer(cmdBuffer);

    std::swap(m_workBatches[batchIndex], m_workBatches.back());
    m_workBatches.pop_back();
}

bool VulkanWorkSubmitInfo::IsAllBatchesFinished() const {
    return m_finishedBatches >= m_workBatches.size();
}

void VulkanWorkSubmitInfo::Dispose(VulkanQueue *vulkanQueue, VulkanWorkAllocatorPool *workAllocatorPool) {
    vulkanQueue->ReturnCommandBufferPool(m_commandBufferPool);
    for (VulkanWorkBatch* workBatch : m_workBatches)
        workBatch->~VulkanWorkBatch();

    m_workBatches.clear();
    workAllocatorPool->ReturnObject(m_workAllocator);
}

VulkanWorkContext::~VulkanWorkContext() {
    m_vulkanDevice->GetWorkAllocatorPool()->ReturnObject(m_workAllocator);
    m_workQueue->ReturnCommandBufferPool(m_commandBufferPool);
}

void VulkanWorkContext::Initialize(VulkanQueue *workQueue, VulkanDevice *vulkanDevice) {
    m_vulkanDevice = vulkanDevice;
    m_workQueue = workQueue;
}

VulkanWorkBatch* VulkanWorkContext::GetWorkBatch(VulkanWorkStage stage) {
    if (m_workBatches.empty() || stage < m_currentStage)
        return CreateWorkBatch();

    return m_workBatches[m_workBatches.size() - 1];
}

void VulkanWorkContext::AddWaitSemaphore(VulkanSemaphore *semaphore, vk::PipelineStageFlags2 waitFlags) {
    VulkanWorkBatch* workBatch = GetWorkBatch(VulkanWorkStage::Wait);
    workBatch->m_waitSemaphores.emplace_back(semaphore);
    workBatch->m_waitSemaphoresFlags.emplace_back(waitFlags);
}

void VulkanWorkContext::AddSignalSemaphore(VulkanSemaphore *semaphore) {
    VulkanWorkBatch* workBatch = GetWorkBatch(VulkanWorkStage::Signal);
    workBatch->m_signalSemaphores.emplace_back(semaphore);
}

VulkanCommandBuffer* VulkanWorkContext::GetCommandBuffer() {
    VulkanWorkBatch* workBatch = GetWorkBatch(VulkanWorkStage::Record);
    if (workBatch->m_commandBuffers.empty()) {
        VulkanCommandBuffer* commandBuffer;
        Photon::Result startResult = StartCommandBuffer(workBatch, commandBuffer);
        return commandBuffer;
    }

    return workBatch->m_commandBuffers[workBatch->m_commandBuffers.size() - 1];
}

Photon::Result VulkanWorkContext::PackWorkBatches(VulkanWorkSubmitInfo& workSubmitInfo) {
    for (VulkanWorkBatch* workBatch : m_workBatches) {
        for (VulkanCommandBuffer* commandBuffer : workBatch->m_commandBuffers) {
            Photon::Result endResult = commandBuffer->End();
            if (endResult != Photon::Result::Success)
                return endResult;
        }

        workBatch->m_batchStatus = VulkanWorkBatchStatus::Packed;
    }

    workSubmitInfo.m_workBatches = std::move(m_workBatches);
    workSubmitInfo.m_commandBufferPool = m_commandBufferPool;
    workSubmitInfo.m_workAllocator = m_workAllocator;

    m_workBatches.clear();
    m_commandBufferPool = nullptr;
    m_workAllocator = nullptr;
    return Photon::Result::Success;
}

VulkanWorkBatch* VulkanWorkContext::CreateWorkBatch() {
    if (m_workAllocator == nullptr) {
        Photon::Result allocatorAcquireResult;
        m_workAllocator = m_vulkanDevice->GetWorkAllocatorPool()->PopObject(allocatorAcquireResult);
    }

    InOutCreateParams<Photon::Result> inOutCreateParams{};
    inOutCreateParams.m_preAllocatedMemory = m_workAllocator->AllocateMemory(sizeof(VulkanWorkBatch));
    VulkanWorkBatch* workBatch = Photon::AllocateObject<VulkanWorkBatch>(&inOutCreateParams);
    workBatch->m_batchStatus = VulkanWorkBatchStatus::Recorded;
    m_workBatches.emplace_back(workBatch);
    m_currentStage = VulkanWorkStage::Wait;
    return workBatch;
}

Photon::Result VulkanWorkContext::StartCommandBuffer(VulkanWorkBatch *vulkanWorkBatch, VulkanCommandBuffer *&commandBuffer) {
    if (m_commandBufferPool == nullptr) {
        Photon::Result acquireResult;
        m_commandBufferPool = m_workQueue->AcquireCommandBufferPool(VulkanCommandBufferType::Primary,
            VulkanCommandBufferLifetime::LongLived, acquireResult);

        if (acquireResult != Photon::Result::Success)
            return acquireResult;
    }

    Photon::Result popResult;
    VulkanCommandBuffer* cmdBuffer = m_commandBufferPool->PopCommandBuffer(popResult);
    if (popResult != Photon::Result::Success)
        return popResult;

    Photon::Result beginResult = commandBuffer->Begin();
    if (beginResult != Photon::Result::Success) {
        m_commandBufferPool->ReturnCommandBuffer(commandBuffer);
        return beginResult;
    }

    vulkanWorkBatch->m_commandBuffers.emplace_back(commandBuffer);
    commandBuffer = cmdBuffer;
    return Photon::Result::Success;
}