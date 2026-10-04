#include "../Public/Math.h"
#include <algorithm>

int Photon::Math::Intersect(const AABB2D& aabbLhs, const AABB2D& aabbRhs) {
    const float overlapX = std::min(aabbLhs.m_maxPoint.x, aabbRhs.m_maxPoint.x)
                         - std::max(aabbLhs.m_minPoint.x, aabbRhs.m_minPoint.x);
    const float overlapY = std::min(aabbLhs.m_maxPoint.y, aabbRhs.m_maxPoint.y)
                         - std::max(aabbLhs.m_minPoint.y, aabbRhs.m_minPoint.y);

    if (overlapX < 0.0f || overlapY < 0.0f) {
        return -1;
    }
    if (overlapX == 0.0f || overlapY == 0.0f) {
        return 0;
    }
    return 1;
}