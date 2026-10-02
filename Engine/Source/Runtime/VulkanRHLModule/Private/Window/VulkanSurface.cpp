#include "../../Public/Window/VulkanSurface.h"
#include "Instance/VulkanInstance.h"
#include <vulkan/vulkan_win32.h>

bool VulkanSurface::Create(const VulkanSurfaceCreateInfo &createInfo) {
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandleA(nullptr);
    surfaceCreateInfo.hwnd = createInfo.m_windowHWND;

    VkSurfaceKHR surfaceHandle;
    VkResult surfaceCreateResult = vkCreateWin32SurfaceKHR(*createInfo.m_vulkanInstance->GetHandle(), &surfaceCreateInfo, nullptr, &surfaceHandle);
    if (surfaceCreateResult != VK_SUCCESS)
        return false;

    m_handle = vk::raii::SurfaceKHR(createInfo.m_vulkanInstance->GetHandle(), surfaceHandle);
    return true;
}

const vk::raii::SurfaceKHR & VulkanSurface::GetHandle() const {
    return m_handle;
}