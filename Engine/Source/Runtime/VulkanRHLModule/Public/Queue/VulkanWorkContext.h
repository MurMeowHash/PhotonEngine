#pragma once

#include <vector>
#include "VulkanWorkBatch.h"
#include "CoreGlobals.h"
#include "FactoryGlobals.h"
#include "CommandBuffers/VulkanCommandBufferPool.h"

class VulkanQueue;
class IAllocator;
class VulkanWorkContextPool;

struct VulkanWorkContextCreateInfo {
    VulkanQueue* m_vulkanQueue;
};

enum class VulkanWorkStage : uint8_t {
    Wait = 0,
    Record = 1,
    Signal = 2,
};

class VulkanWorkContext {
    friend class VulkanWorkContextPool;
public:
    ~VulkanWorkContext();
    static VulkanWorkContext* Create(const VulkanWorkContextCreateInfo& createInfo, InOutCreateParams<Photon::Result>* inOutCreateParams = nullptr);
    VulkanWorkBatch* GetWorkBatch(VulkanWorkStage stage);
    void AddWaitSemaphore(VulkanSemaphore* semaphore, vk::PipelineStageFlags2 waitFlags);
    void AddSignalSemaphore(VulkanSemaphore* semaphore);
    [[nodiscard]] Photon::Result GetCommandBuffer(VulkanCommandBuffer*& commandBuffer);
    [[nodiscard]] Photon::Result PackWorkBatches();
    [[nodiscard]] std::vector<VulkanWorkBatch*> GetAllWorkBatches() const;
    void DisposePackedBatch(size_t batchIndex);
    [[nodiscard]] bool HasAnyBatches() const;
private:
    VulkanQueue* m_vulkanQueue = nullptr;

    std::vector<VulkanWorkBatch*> m_workBatches;
    VulkanWorkStage m_currentStage{};
    VulkanCommandBufferPool* m_commandBufferPool = nullptr;
    IAllocator* m_workAllocator = nullptr;
    bool m_isBatchesPacked{};

    VulkanWorkBatch* CreateWorkBatch();
    [[nodiscard]] Photon::Result StartCommandBuffer(VulkanWorkBatch* vulkanWorkBatch, VulkanCommandBuffer*& commandBuffer) const;
    void RemovePackedBatch(size_t batchIndex);

    void ClearBatches() const;
};