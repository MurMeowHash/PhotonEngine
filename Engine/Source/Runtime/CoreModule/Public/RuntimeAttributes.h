#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

class RuntimeAttributes {
public:
    template<typename T> requires (sizeof(T) <= sizeof(uint64_t)) && std::is_trivially_copyable_v<T>
    void OverrideAttribute(const std::string& attributeType, T attributeValue) {
        uint64_t stagingValue = 0;
        memcpy(&stagingValue, &attributeValue, sizeof(T));
        m_attributes[attributeType] = stagingValue;
    }

    template<typename T> requires (sizeof(T) <= sizeof(uint64_t)) && std::is_trivially_copyable_v<T>
    bool TryRetrieveAttribute(const std::string& attributeType, T& attributeValue) const {
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
    std::unordered_map<std::string, uint64_t> m_attributes;
};