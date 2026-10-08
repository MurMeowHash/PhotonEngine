#include "../../Public/Queue/VulkanWorkContext.h"
#include "LinearAllocator.h"

VulkanWorkContext::~VulkanWorkContext() {
    ClearBatches();
    delete m_workAllocator;
    m_vulkanQueue->ReturnCommandBufferPool(m_commandBufferPool);
}

VulkanWorkContext* VulkanWorkContext::Create(const VulkanWorkContextCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    Photon::Result acquireResult;
    VulkanCommandBufferPool* commandBufferPool =
        createInfo.m_vulkanQueue->AcquireCommandBufferPool(VulkanCommandBufferType::Primary, VulkanCommandBufferLifetime::LongLived, acquireResult);

    if (acquireResult != Photon::Result::Success) {
        Photon::PushResult(acquireResult, inOutCreateParams);
        return nullptr;
    }

    VulkanWorkContext* instance = Photon::AllocateObject<VulkanWorkContext>(inOutCreateParams);
    instance->m_vulkanQueue = createInfo.m_vulkanQueue;
    instance->m_commandBufferPool = commandBufferPool;
    instance->m_workAllocator = new LinearAllocator(sizeof(VulkanWorkBatch) * 8ull);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
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

Photon::Result VulkanWorkContext::GetCommandBuffer(VulkanCommandBuffer *&commandBuffer) {
    VulkanWorkBatch* workBatch = GetWorkBatch(VulkanWorkStage::Record);
    if (workBatch->m_commandBuffers.empty()) {
        Photon::Result startResult = StartCommandBuffer(workBatch, commandBuffer);
        return startResult;
    }

    commandBuffer = workBatch->m_commandBuffers[workBatch->m_commandBuffers.size() - 1];
    return Photon::Result::Success;
}

Photon::Result VulkanWorkContext::PackWorkBatches() {
    for (VulkanWorkBatch* workBatch : m_workBatches) {
        for (VulkanCommandBuffer* commandBuffer : workBatch->m_commandBuffers) {
            Photon::Result endResult = commandBuffer->End();
            if (endResult != Photon::Result::Success)
                return endResult;
        }
    }

    m_isBatchesPacked = true;
    return Photon::Result::Success;
}

std::vector<VulkanWorkBatch*> VulkanWorkContext::GetAllWorkBatches() const {
    return m_workBatches;
}

void VulkanWorkContext::DisposePackedBatch(size_t batchIndex) {
    if (!m_isBatchesPacked || batchIndex >= m_workBatches.size())
        return;

    for (VulkanCommandBuffer* commandBuffer : m_workBatches[batchIndex]->m_commandBuffers)
        m_commandBufferPool->ReturnCommandBuffer(commandBuffer);

    RemovePackedBatch(batchIndex);
}

bool VulkanWorkContext::HasAnyBatches() const {
    return !m_workBatches.empty();
}

VulkanWorkBatch* VulkanWorkContext::CreateWorkBatch() {
    InOutCreateParams<Photon::Result> inOutCreateParams{};
    inOutCreateParams.m_preAllocatedMemory = m_workAllocator->AllocateMemory(sizeof(VulkanWorkBatch));
    VulkanWorkBatch* workBatch = Photon::AllocateObject<VulkanWorkBatch>(&inOutCreateParams);
    m_workBatches.emplace_back(workBatch);
    m_currentStage = VulkanWorkStage::Wait;
    return workBatch;
}

Photon::Result VulkanWorkContext::StartCommandBuffer(VulkanWorkBatch *vulkanWorkBatch, VulkanCommandBuffer *&commandBuffer) const {
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

void VulkanWorkContext::RemovePackedBatch(size_t batchIndex) {
    if (!m_isBatchesPacked || m_workBatches.empty())
        return;

    std::swap(m_workBatches[batchIndex], m_workBatches[m_workBatches.size() - 1]);
    m_workBatches.pop_back();
}

void VulkanWorkContext::ClearBatches() const {
    for (VulkanWorkBatch* workBatch: m_workBatches)
        workBatch->~VulkanWorkBatch();

    m_workAllocator->FreeMemory();
}
