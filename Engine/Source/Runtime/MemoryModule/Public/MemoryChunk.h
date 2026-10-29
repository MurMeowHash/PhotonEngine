#pragma once

#include <concepts>
#include "Memory.h"
#include "MemoryOperationsCommon.h"

template<typename TSize, std::derived_from<Memory<TSize>> TMemory>
class MemoryChunk {
public:
    virtual ~MemoryChunk() = default;

    [[nodiscard]] bool TryAllocate(TSize size, TSize alignment, TMemory& outMemory) {
        TSize alignedOffset = Photon::MemoryOperationsCommon::AlignForward(m_offset, alignment);
        if (size + alignedOffset > m_memory.GetSize())
            return false;

        MemoryInitializeInfo<TSize> memInitializeInfo{};
        memInitializeInfo.m_offset = alignedOffset;
        memInitializeInfo.m_alignmentOffset = alignedOffset - m_offset;
        memInitializeInfo.m_size = size;
        InitializeMemoryObject(outMemory, memInitializeInfo);

        m_offset = alignedOffset + size;
        return true;
    }

    void InvalidateMemory() {
        m_offset = 0;
    }

protected:
    TMemory m_memory;

    virtual void InitializeMemoryObject(TMemory& memoryObject, const MemoryInitializeInfo<TSize>& memInitializeInfo) = 0;

private:
    TSize m_offset{};
    MemoryChunk* m_next = nullptr;

    template<typename TFriendSize, std::derived_from<Memory<TFriendSize>> TFriendMemory,
        std::derived_from<MemoryChunk<TFriendSize, TFriendMemory>> TFriendChunk>
    friend class MemoryChunkExtractedAllocator;
};

template<typename TSize, std::derived_from<Memory<TSize>> TMemory, std::derived_from<MemoryChunk<TSize, TMemory>> TMemChunk>
class MemoryChunkExtractedAllocatorBase {
public:
    virtual ~MemoryChunkExtractedAllocatorBase() = default;

    virtual void FreeAll() {
        delete m_current;
        m_current = nullptr;
    }

protected:
    TMemChunk* m_current = nullptr;

    virtual void OnChunkExtracted(TMemChunk* chunk) = 0;

    TMemory AllocateFromNewChunk(TMemChunk* chunk, TSize size, TSize alignment) {
        TMemory outMemory{};
        if (chunk == nullptr || !chunk->TryAllocate(size, alignment, outMemory))
            return TMemory{};

        if (m_current != nullptr)
            OnChunkExtracted(m_current);

        m_current = chunk;
        return outMemory;
    }
};

template<typename TSize, std::derived_from<Memory<TSize>> TMemory, std::derived_from<MemoryChunk<TSize, TMemory>> TMemChunk>
class MemoryChunkExtractedAllocator : public MemoryChunkExtractedAllocatorBase<TSize, TMemory, TMemChunk> {
public:
    ~MemoryChunkExtractedAllocator() override {
        MemoryChunkExtractedAllocator::FreeAll();
    }

    void FreeAll() final override {
        MemoryChunkExtractedAllocatorBase<TSize, TMemory, TMemChunk>::FreeAll();

        MemoryChunk<TSize, TMemory>* inspectedChunk = m_freeRoot;
        while (inspectedChunk != nullptr) {
            MemoryChunk<TSize, TMemory>* chunkToDelete = inspectedChunk;
            inspectedChunk = inspectedChunk->m_next;
            delete chunkToDelete;
        }

        m_freeRoot = nullptr;
    }

    explicit MemoryChunkExtractedAllocator(TSize chunkSize)
    : m_chunkSize(chunkSize) {}

    TMemory Allocate(TSize size, TSize alignment) {
        TMemory outMemory{};
        if (this->m_current != nullptr && this->m_current->TryAllocate(size, alignment, outMemory))
            return outMemory;

        if (m_freeRoot == nullptr)
            return this->AllocateFromNewChunk(AllocateMemoryChunk(m_chunkSize), size, alignment);

        TMemChunk* freeChunk = m_freeRoot;
        m_freeRoot = static_cast<TMemChunk*>(m_freeRoot->m_next);
        return this->AllocateFromNewChunk(freeChunk, size, alignment);
    }

    void AddFreeChunk(TMemChunk* chunk) {
        chunk->m_next = m_freeRoot;
        m_freeRoot = chunk;
    }

protected:
    TMemChunk* m_freeRoot = nullptr;

    virtual TMemChunk* AllocateMemoryChunk(TSize chunkSize) = 0;

private:
    TSize m_chunkSize;
};

template<typename TSize, std::derived_from<Memory<TSize>> TMemory, std::derived_from<MemoryChunk<TSize, TMemory>> TMemChunk, typename... TChunkArgs>
class MemoryChunkExtractedAllocatorSeparate : public MemoryChunkExtractedAllocatorBase<TSize, TMemory, TMemChunk> {
public:
    void FreeAll() final override {
        MemoryChunkExtractedAllocatorBase<TSize, TMemory, TMemChunk>::FreeAll();
    }

    ~MemoryChunkExtractedAllocatorSeparate() override {
        MemoryChunkExtractedAllocatorSeparate::FreeAll();
    }

    TMemory Allocate(TSize size, TSize alignment, const TChunkArgs&... chunkArgs) {
        return this->AllocateFromNewChunk(AllocateMemoryChunk(size, chunkArgs...), size, alignment);
    }

protected:
    virtual TMemChunk* AllocateMemoryChunk(TSize chunkSize, const TChunkArgs&... chunkArgs) = 0;
};