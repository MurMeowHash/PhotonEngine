#pragma once

#include <vector>
#include "VulkanWorkBatch.h"
#include "CoreGlobals.h"
#include "CommandBuffers/VulkanCommandBufferPool.h"

class VulkanQueue;
class IAllocator;
class VulkanDevice;

enum class VulkanWorkStage : uint8_t {
    Wait = 0,
    Record = 1,
    Signal = 2,
};

struct VulkanWorkSubmitInfo {
    std::vector<VulkanWorkBatch*> m_workBatches;
    VulkanCommandBufferPool* m_commandBufferPool;
    IAllocator* m_workAllocator;
    size_t m_finishedBatches;

    void FinishBatch(size_t batchIndex);
    void DisposeBatch(size_t batchIndex);
    [[nodiscard]] bool IsAllBatchesFinished() const;
    void Dispose(VulkanQueue* vulkanQueue, VulkanWorkAllocatorPool* workAllocatorPool);
};

class VulkanWorkContext {
public:
    virtual ~VulkanWorkContext();
    void Initialize(VulkanQueue* workQueue, VulkanDevice* vulkanDevice);
    VulkanWorkBatch* GetWorkBatch(VulkanWorkStage stage);
    void AddWaitSemaphore(VulkanSemaphore* semaphore, vk::PipelineStageFlags2 waitFlags);
    void AddSignalSemaphore(VulkanSemaphore* semaphore);
    [[nodiscard]] VulkanCommandBuffer* GetCommandBuffer();
    [[nodiscard]] Photon::Result PackWorkBatches(VulkanWorkSubmitInfo& workSubmitInfo);

protected:
    VulkanQueue* m_workQueue = nullptr;

private:
    VulkanDevice* m_vulkanDevice = nullptr;

    std::vector<VulkanWorkBatch*> m_workBatches;
    VulkanWorkStage m_currentStage{};
    VulkanCommandBufferPool* m_commandBufferPool = nullptr;
    IAllocator* m_workAllocator = nullptr;

    VulkanWorkBatch* CreateWorkBatch();
    [[nodiscard]] Photon::Result StartCommandBuffer(VulkanWorkBatch* vulkanWorkBatch, VulkanCommandBuffer*& commandBuffer);
};