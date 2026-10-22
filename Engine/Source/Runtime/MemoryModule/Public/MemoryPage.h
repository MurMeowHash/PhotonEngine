#pragma once

#include <concepts>
#include "Memory.h"
#include "MemoryRange.h"
#include <vector>

template<typename TSize, std::derived_from<Memory<TSize>> TMemory>
class MemoryPage {
public:
    virtual ~MemoryPage() = default;

    explicit MemoryPage() = default;

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
class MemoryPagesAllocator {
public:
    virtual ~MemoryPagesAllocator() = default;

    MemoryPagesAllocator(TSize pageSize, bool pooledPages, bool deallocUnusedPages) :
    m_pageSize(pageSize), m_pooledPages(pooledPages), m_deallocUnusedPages(deallocUnusedPages) { }

    TMemory Allocate(TSize memSize, TSize alignment) {
        if (!m_pooledPages)
            return AllocateMemoryFromNewPage(memSize, memSize, alignment);

        for (TMemPage& page : m_pages) {
            TMemory outMemory{};
            if (page.TryAllocateFromRange(memSize, alignment, outMemory))
                return outMemory;
        }

        return AllocateMemoryFromNewPage(m_pageSize, memSize, alignment);
    }

    void Free(TMemory& memory) {
        auto pageToReturnMemory = std::ranges::find_if(m_pages,
        [&memory, this](const TMemPage& memoryPage) {
            return this->MemoryBelongsToPage(memoryPage, memory);
        });

        if (pageToReturnMemory == m_pages.end())
            return;

        pageToReturnMemory->ReturnMemory(memory);
        if (pageToReturnMemory->IsEmpty() && m_deallocUnusedPages) {
            std::swap(*pageToReturnMemory, m_pages.back());
            m_pages.pop_back();
        }

        memory.InvalidateMemory();
    }

    void FreeAll() {
        m_pages.clear();
    }

protected:
    virtual TMemPage AllocatePage(TSize pageSize) = 0;
    virtual bool MemoryBelongsToPage(const TMemPage& memoryPage, const TMemory& memory) = 0;

private:
    std::vector<TMemPage> m_pages;
    TSize m_pageSize;
    bool m_pooledPages;
    bool m_deallocUnusedPages;

    TMemory AllocateMemoryFromNewPage(TSize pageSize, TSize memSize, TSize alignment) {
        TMemPage allocPage = AllocatePage(pageSize);
        TMemory outMemory{};
        if (allocPage.TryAllocateFromRange(memSize, alignment, outMemory))
            m_pages.push_back(allocPage);

        return outMemory;
    }
};