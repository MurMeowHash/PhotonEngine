#pragma once

#include "PhotonPool.h"
#include <memory>

template<typename T>
class PhotonSafePool : protected PhotonPool<T> {
public:
    [[nodiscard]] std::shared_ptr<T> PopObjectSafe(Photon::Result& result) {
        T* pooledObject = this->PopObject(result);
        if (result != Photon::Result::Success)
            return nullptr;

        return std::shared_ptr<T>(pooledObject, [this](T* object){ this->ReturnObject(object); });
    }

    void ReturnObjectSafe(std::shared_ptr<T>& object) {
        object.reset();
    }
};