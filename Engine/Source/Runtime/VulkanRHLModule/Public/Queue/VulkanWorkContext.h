#pragma once

#include <vector>
#include "VulkanWorkBatch.h"
#include "CoreGlobals.h"
#include "CommandBuffers/VulkanCommandBufferPool.h"
#include <memory>

class VulkanQueue;
class IAllocator;
class VulkanDevice;
class VulkanWorkAllocatorPool;

enum class VulkanWorkStage : uint8_t {
    Wait = 0,
    Record = 1,
    Signal = 2,
};

struct VulkanWorkSubmitInfo {
    std::vector<VulkanWorkBatch*> m_workBatches;
    VulkanCommandBufferPool* m_commandBufferPool = nullptr;
    IAllocator* m_workAllocator = nullptr;
    size_t m_finishedBatches{};

    void FinishBatch(size_t batchIndex);
    void DisposeBatch(size_t batchIndex);
    [[nodiscard]] bool IsAllBatchesFinished() const;
    void Dispose(VulkanQueue* vulkanQueue, VulkanWorkAllocatorPool* workAllocatorPool);
    [[nodiscard]] uint64_t GetBatchMaxTimelineValue(VulkanWorkBatchStatus batchStatus) const;
};

class VulkanWorkContext {
public:
    virtual ~VulkanWorkContext();
    void Initialize(VulkanQueue* workQueue, VulkanDevice* vulkanDevice);
    VulkanWorkBatch* GetWorkBatch(VulkanWorkStage stage);
    void AddWaitSemaphore(std::shared_ptr<VulkanSemaphore> semaphore, vk::PipelineStageFlags2 waitFlags);
    void AddSignalSemaphore(std::shared_ptr<VulkanSemaphore> semaphore);
    [[nodiscard]] VulkanCommandBuffer* GetCommandBuffer();
    [[nodiscard]] Photon::Result PackWorkBatches(VulkanWorkSubmitInfo& workSubmitInfo);

protected:
    VulkanQueue* m_workQueue = nullptr;
    VulkanDevice* m_vulkanDevice = nullptr;

private:

    std::vector<VulkanWorkBatch*> m_workBatches;
    VulkanWorkStage m_currentStage{};
    VulkanCommandBufferPool* m_commandBufferPool = nullptr;
    IAllocator* m_workAllocator = nullptr;

    VulkanWorkBatch* CreateWorkBatch();
    [[nodiscard]] Photon::Result StartCommandBuffer(VulkanWorkBatch* vulkanWorkBatch, VulkanCommandBuffer*& commandBuffer);
};