#pragma once

#include <vector>
#include <cstdint>

class VulkanSpirV {
public:
    explicit VulkanSpirV(const std::vector<char>& code);
    [[nodiscard]] size_t GetCodeSize() const;
    [[nodiscard]] const uint32_t* GetCode() const;
private:
    std::vector<char> m_code;
};