#pragma once

#include "glm/glm.hpp"

template<int TLength>
struct AABB {
    glm::vec<TLength, float, glm::defaultp> m_minPoint;
    glm::vec<TLength, float, glm::defaultp> m_maxPoint;
};

struct AABB2D : AABB<2> {
    AABB2D(float minX, float maxX, float minY, float maxY)
    : AABB() {
        m_minPoint = glm::vec2(minX, minY);
        m_maxPoint = glm::vec2(maxX, maxY);
    }
};

namespace Photon::Math {
    int Intersect(const AABB2D& aabbLhs, const AABB2D& aabbRhs);
};