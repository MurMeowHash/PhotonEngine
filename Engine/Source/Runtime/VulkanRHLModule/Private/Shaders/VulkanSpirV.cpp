#include "../../Public/Shaders/VulkanSpirV.h"

VulkanSpirV::VulkanSpirV(const std::vector<char> &code)
: m_code(code) { }

size_t VulkanSpirV::GetCodeSize() const {
    return m_code.size() * sizeof(char);
}

const uint32_t* VulkanSpirV::GetCode() const {
    return reinterpret_cast<const uint32_t*>(m_code.data());
}