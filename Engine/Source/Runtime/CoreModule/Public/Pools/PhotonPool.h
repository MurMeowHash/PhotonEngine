#pragma once

#include <queue>
#include "CoreGlobals.h"
#include <cstdint>
#include "FactoryGlobals.h"
#include "IAllocator.h"
#include "SimpleAllocator.h"

template<typename T>
class PhotonPool {
public:
    virtual ~PhotonPool() {
        while (!m_pool.empty()) {
            T* object = m_pool.front();
            object->~T();
            m_pool.pop();
        }

        m_poolAllocator->FreeMemory();
        delete m_poolAllocator;
    }

    [[nodiscard]] T* PopObject(Photon::Result& result) {
        if (m_pool.empty()) {
            Photon::Result extendResult = TryExtendPool();
            if (extendResult != Photon::Result::Success) {
                result = extendResult;
                return nullptr;
            }
        }

        T* pooledObject = m_pool.front();
        m_pool.pop();
        result = Photon::Result::Success;
        return pooledObject;
    }

    void ReturnObject(T* object) {
        DisposeObject(object);
        m_pool.emplace(object);
    }

protected:
    void Initialize(uint32_t initialPoolSize, uint32_t maxPoolSize, IAllocator* allocator = nullptr) {
        m_currentPoolSize = 0;
        m_maxPoolSize = maxPoolSize;

        if (allocator == nullptr)
            m_poolAllocator = new SimpleAllocator(initialPoolSize);
        else
            m_poolAllocator = allocator;

        for (uint32_t i = 0; i < initialPoolSize; i++)
            TryExtendPool();
    }

    virtual T* CreateObject(InOutCreateParams<Photon::Result>* inOutCreateParams) = 0;
    virtual void DisposeObject([[maybe_unused]] T* object) { }

private:
    std::queue<T*> m_pool;
    uint32_t m_currentPoolSize{};
    uint32_t m_maxPoolSize{};

    IAllocator* m_poolAllocator = nullptr;


    Photon::Result TryExtendPool() {
        if (m_currentPoolSize >= m_maxPoolSize)
            return Photon::Result::UnknownFailure;

        InOutCreateParams<Photon::Result> inOutCreateParams{};
        inOutCreateParams.m_preAllocatedMemory = m_poolAllocator->AllocateMemory(sizeof(T));
        T* object = CreateObject(&inOutCreateParams);
        if (inOutCreateParams.m_result != Photon::Result::Success)
            return Photon::Result::UnknownFailure;

        m_pool.emplace(object);
        ++m_currentPoolSize;
        return Photon::Result::Success;
    }
};