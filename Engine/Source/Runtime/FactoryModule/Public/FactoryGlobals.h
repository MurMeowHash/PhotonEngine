#pragma once

#include <type_traits>
#include <new>

template<typename TResult> requires std::is_enum_v<TResult>
struct InOutCreateParams {
    TResult m_result;
    void* m_preAllocatedMemory = nullptr;
};

namespace Photon {
    template<typename TObject, typename TResult> requires std::is_enum_v<TResult>
    inline TObject* AllocateObject(InOutCreateParams<TResult>* createParams) {
        if (createParams == nullptr || createParams->m_preAllocatedMemory == nullptr)
            return new TObject();
        else
            return new (createParams->m_preAllocatedMemory) TObject();
    }

    template<typename TResult> requires std::is_enum_v<TResult>
    inline void PushResult(TResult result, InOutCreateParams<TResult>* createParams) {
        if (createParams == nullptr)
            return;

        createParams->m_result = result;
    }
}