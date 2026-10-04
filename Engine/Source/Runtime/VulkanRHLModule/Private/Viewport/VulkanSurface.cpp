#include "../../Public/Viewport/VulkanSurface.h"
#include "Instance/VulkanInstance.h"
#include <vulkan/vulkan_win32.h>

VulkanSurface * VulkanSurface::Create(const VulkanSurfaceCreateInfo &createInfo, InOutCreateParams<Photon::Result> *inOutCreateParams) {
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandleA(nullptr);
    surfaceCreateInfo.hwnd = createInfo.m_windowHWND;
    surfaceCreateInfo.pNext = nullptr;

    VkSurfaceKHR surfaceHandle;
    VkResult surfaceCreateResult = vkCreateWin32SurfaceKHR(*createInfo.m_vulkanInstance->GetHandle(), &surfaceCreateInfo, nullptr, &surfaceHandle);
    if (surfaceCreateResult != VK_SUCCESS) {
        Photon::PushResult(Photon::Result::UnknownFailure, inOutCreateParams);
        return nullptr;
    }

    VulkanSurface* instance = Photon::AllocateObject<VulkanSurface>(inOutCreateParams);
    instance->m_handle = vk::raii::SurfaceKHR(createInfo.m_vulkanInstance->GetHandle(), surfaceHandle);
    Photon::PushResult(Photon::Result::Success, inOutCreateParams);
    return instance;
}

const vk::raii::SurfaceKHR & VulkanSurface::GetHandle() const {
    return m_handle;
}