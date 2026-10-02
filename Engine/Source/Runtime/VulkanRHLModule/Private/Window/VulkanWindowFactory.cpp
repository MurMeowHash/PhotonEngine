#include "../../Public/Window/VulkanWindowFactory.h"

VulkanSurface * Photon::Vulkan::WindowFactory::CreateVulkanSurface(const VulkanSurfaceCreateInfo &createInfo, bool *isValid) {
    VulkanSurface* surface = new VulkanSurface();
    bool isCreated = surface->Create(createInfo);
    if (isValid)
        *isValid = isCreated;

    return surface;
}