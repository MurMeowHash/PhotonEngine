#pragma once

#include <unordered_map>
#include <string_view>
#include <cstring>
#include <cstdint>

class RuntimeAttributes {
public:
    template<typename T> requires (sizeof(T) <= sizeof(uint64_t)) && std::is_trivially_copyable_v<T>
    void OverrideAttribute(const char* attributeType, T attributeValue) {
        uint64_t stagingValue = 0;
        memcpy(&stagingValue, &attributeValue, sizeof(T));
        m_attributes[attributeType] = stagingValue;
    }

    template<typename T> requires (sizeof(T) <= sizeof(uint64_t)) && std::is_trivially_copyable_v<T>
    bool TryRetrieveAttribute(const char* attributeType, T& attributeValue) const {
        auto attributeIterator = m_attributes.find(attributeType);
        if (attributeIterator == m_attributes.end())
            return false;

        memcpy(&attributeValue, &attributeIterator->second, sizeof(T));
        return true;
    }

    void Invalidate() {
        m_attributes.clear();
    }

private:
    struct AttributeTypeHasher {
        size_t operator()(const char* attributeType) const {
            return std::hash<std::string_view>{}(attributeType);
        }
    };

    struct AttributeTypeComparator {
        bool operator()(const char* lhs, const char* rhs) const {
            return std::string_view(lhs) == std::string_view(rhs);
        }
    };

    std::unordered_map<const char*, uint64_t, AttributeTypeHasher, AttributeTypeComparator> m_attributes;
};
