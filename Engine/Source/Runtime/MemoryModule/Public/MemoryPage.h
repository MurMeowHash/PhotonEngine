#pragma once

#include <algorithm>
#include <concepts>
#include <memory>
#include "Memory.h"
#include "MemoryRange.h"
#include <vector>

template<typename TSize, std::derived_from<Memory<TSize>> TMemory>
class MemoryPage {
public:
    virtual ~MemoryPage() = default;

    explicit MemoryPage() = default;

    [[nodiscard]] virtual bool OwnsMemory(const TMemory& memory) const = 0;

    [[nodiscard]] virtual bool TryAllocateFromRange(TSize requiredMemorySize, TSize alignment, TMemory& allocatedMemory) {
        auto freeRange = std::ranges::find_if(m_freeRanges,
            [requiredMemorySize, alignment] (const MemoryRange<TSize>& memoryRange) {
                return requiredMemorySize <= memoryRange.GetAlignedSize(alignment);
        });

        if (freeRange == m_freeRanges.end())
            return false;

        MemoryInitializeInfo<TSize> memInitializeInfo{};
        memInitializeInfo.m_size = requiredMemorySize;
        memInitializeInfo.m_offset = freeRange->GetAlignedBegin(alignment);
        memInitializeInfo.m_alignmentOffset = freeRange->GetAlignedBegin(alignment) - freeRange->GetBegin();
        InitializeMemoryObject(allocatedMemory, memInitializeInfo);

        if (requiredMemorySize >= freeRange->GetAlignedSize(alignment))
            m_freeRanges.erase(freeRange);
        else
            freeRange->ShrinkForwardAligned(requiredMemorySize, alignment);

        return true;
    }

    void ReturnMemory(TMemory& memory) {
        TSize occupationOffset = memory.GetMemoryOccupationOffset();
        MemoryRange<TSize> returnedRange = MemoryRange<TSize>(occupationOffset, occupationOffset + memory.GetAllocationSize());
        auto returnedRangePosInterator = std::ranges::lower_bound(m_freeRanges, returnedRange, MemoryRangeComparator());
        auto returnedRangeIterator = m_freeRanges.insert(returnedRangePosInterator, returnedRange);
        MergeRangesForward(returnedRangeIterator, 1);
        MergeRangesBackward(returnedRangeIterator, 1);

        std::erase_if(m_freeRanges, [](const MemoryRange<TSize>& memoryRange) {
            return memoryRange.IsCoalesced();
        });

        memory.InvalidateMemory();
    }

    [[nodiscard]] TMemory GetPageMemory() const {
        return m_memory;
    }

    void InvalidateMemory() {
        m_freeRanges.clear();
        m_freeRanges.emplace_back(0, m_memory.GetAllocationSize());
    }

    [[nodiscard]] bool IsEmpty() const {
        if (m_freeRanges.size() < 1)
            return false;

        return m_freeRanges[0] == MemoryRange<TSize>(0, m_memory.GetAllocationSize());
    }

protected:
    TMemory m_memory;

    virtual void InitializeMemoryObject(TMemory& memoryObject, const MemoryInitializeInfo<TSize>& memInitializeInfo) = 0;

private:
    using RangeIteratorOffset = std::iter_difference_t<typename std::vector<MemoryRange<TSize>>::iterator>;

    struct MemoryRangeComparator {
        bool operator()(const MemoryRange<TSize>& lhs, const MemoryRange<TSize>& rhs) const {
            return lhs.GetBegin() < rhs.GetBegin();
        }
    };

    std::vector<MemoryRange<TSize>> m_freeRanges;

    void MergeRangesForward(std::vector<MemoryRange<TSize>>::iterator currentRange, RangeIteratorOffset neighborOffset) {
        auto neighborRange = currentRange + neighborOffset;

        if (neighborRange == m_freeRanges.end())
            return;

        if (currentRange->GetEnd() == neighborRange->GetBegin()) {
            currentRange->OverrideEnd(neighborRange->GetEnd());
            neighborRange->MarkCoalesced();
            MergeRangesForward(currentRange, neighborOffset + 1);
        }
    }

    void MergeRangesBackward(std::vector<MemoryRange<TSize>>::iterator currentRange, RangeIteratorOffset neighborOffset) {
        auto neighborRange = currentRange - neighborOffset;

        if (neighborRange < m_freeRanges.begin())
            return;

        if (currentRange->GetBegin() == neighborRange->GetEnd()) {
            currentRange->OverrideBegin(neighborRange->GetBegin());
            neighborRange->MarkCoalesced();
            MergeRangesBackward(currentRange, neighborOffset + 1);
        }
    }
};

template<typename TSize, std::derived_from<Memory<TSize>> TMemory, std::derived_from<MemoryPage<TSize, TMemory>> TMemPage>
class MemoryPagesAllocatorBase {
public:
    virtual ~MemoryPagesAllocatorBase() = default;

    explicit MemoryPagesAllocatorBase(bool releaseEmptyPages)
    : m_releaseEmptyPages(releaseEmptyPages) { }

    MemoryPagesAllocatorBase(const MemoryPagesAllocatorBase&) = delete;
    MemoryPagesAllocatorBase& operator=(const MemoryPagesAllocatorBase&) = delete;
    MemoryPagesAllocatorBase(MemoryPagesAllocatorBase&&) noexcept = default;
    MemoryPagesAllocatorBase& operator=(MemoryPagesAllocatorBase&&) noexcept = default;

    void Free(TMemory& memory) {
        auto pageToReturnMemory = std::ranges::find_if(m_pages,
            [&memory](const std::unique_ptr<TMemPage>& memoryPage) {
                return memoryPage->OwnsMemory(memory);
        });

        if (pageToReturnMemory == m_pages.end())
            return;

        (*pageToReturnMemory)->ReturnMemory(memory);
        if ((*pageToReturnMemory)->IsEmpty() && m_releaseEmptyPages) {
            std::swap(*pageToReturnMemory, m_pages.back());
            m_pages.pop_back();
        }
    }

    void FreeAll() {
        m_pages.clear();
    }

protected:
    std::vector<std::unique_ptr<TMemPage>> m_pages;

    TMemory AllocateFromNewPage(std::unique_ptr<TMemPage> memoryPage, TSize memSize, TSize alignment) {
        TMemory outMemory{};
        if (memoryPage == nullptr || !memoryPage->TryAllocateFromRange(memSize, alignment, outMemory))
            return outMemory;

        m_pages.push_back(std::move(memoryPage));
        return outMemory;
    }

private:
    bool m_releaseEmptyPages;
};

template<typename TSize, std::derived_from<Memory<TSize>> TMemory, std::derived_from<MemoryPage<TSize, TMemory>> TMemPage>
class MemoryPagesAllocator : public MemoryPagesAllocatorBase<TSize, TMemory, TMemPage> {
public:
    MemoryPagesAllocator(TSize pageSize, bool releaseEmptyPages)
    : MemoryPagesAllocatorBase<TSize, TMemory, TMemPage>(releaseEmptyPages), m_pageSize(pageSize) { }

    TMemory Allocate(TSize memSize, TSize alignment) {
        for (std::unique_ptr<TMemPage>& memoryPage : this->m_pages) {
            TMemory outMemory{};
            if (memoryPage->TryAllocateFromRange(memSize, alignment, outMemory))
                return outMemory;
        }

        return this->AllocateFromNewPage(AllocatePage(m_pageSize), memSize, alignment);
    }

    [[nodiscard]] TSize GetPageSize() const {
        return m_pageSize;
    }

protected:
    [[nodiscard]] virtual std::unique_ptr<TMemPage> AllocatePage(TSize pageSize) = 0;

private:
    TSize m_pageSize;
};

template<typename TSize, std::derived_from<Memory<TSize>> TMemory, std::derived_from<MemoryPage<TSize, TMemory>> TMemPage,
    typename... TPageArgs>
class MemoryPagesAllocatorSeparate : public MemoryPagesAllocatorBase<TSize, TMemory, TMemPage> {
public:
    MemoryPagesAllocatorSeparate()
    : MemoryPagesAllocatorBase<TSize, TMemory, TMemPage>(true) { }

    TMemory Allocate(TSize memSize, TSize alignment, const TPageArgs&... pageArgs) {
        return this->AllocateFromNewPage(AllocatePage(memSize, pageArgs...), memSize, alignment);
    }

protected:
    [[nodiscard]] virtual std::unique_ptr<TMemPage> AllocatePage(TSize pageSize, const TPageArgs&... pageArgs) = 0;
};