#include "../../Public/Instance/VulkanDebugger.h"
#include "CoreUtils.h"
#include "Logger.h"
#include "../Public/Instance/VulkanInstance.h"

bool VulkanDebugger::Create(const VulkanDebuggerCreateInfo &createInfo) {
    if (!createInfo.m_vulkanInstance->IsExtensionSupported(vk::EXTDebugUtilsExtensionName))
        return false;

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
    debugUtilsMessengerCreateInfoEXT.messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    debugUtilsMessengerCreateInfoEXT.messageType =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    debugUtilsMessengerCreateInfoEXT.pfnUserCallback = VulkanDebugCallback;

    vk::ResultValue<vk::raii::DebugUtilsMessengerEXT> debugMessangerWrapper =
        createInfo.m_vulkanInstance->GetHandle().createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);

    if(debugMessangerWrapper.result != vk::Result::eSuccess)
        return false;

    m_handle = std::move(debugMessangerWrapper.value);
    return true;
}

vk::Bool32 VulkanDebugger::VulkanDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
    std::string typeStr = (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation) ? "Validation" :
                          (type & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance) ? "Performance" : "General";

    std::string message = StringFormatter(
            "Vulkan ", typeStr, " [",
            pCallbackData->pMessageIdName ? pCallbackData->pMessageIdName : "Unknown",
            "]: ",
            pCallbackData->pMessage ? pCallbackData->pMessage : "No message provided"
    );

    if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
        Photon::Logger::PrintError(message);
    } else if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        Photon::Logger::PrintWarning(message);
    } else {
        Photon::Logger::PrintInfo(message);
    }

    return VK_FALSE;
}
