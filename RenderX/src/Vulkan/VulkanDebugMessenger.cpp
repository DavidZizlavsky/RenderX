#include "RenderX/Vulkan/VulkanDebugMessenger.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>

namespace RenderX {
    // Static function used as a callback for the debug messenger
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) 
    {
        // Determine severity of current message
        std::string severity = "UNKNOWN";
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            severity = "VERBOSE";
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            severity = "INFO";
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            severity = "WARNING";
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            severity = "ERROR";
        }

        // Prints message from callback to error stream
        Logger::VulkanDebug(pCallbackData->pMessage, severity);
        return VK_FALSE;
    }

	bool VulkanDebugMessenger::Initialize(VkInstance instance) {
        // Gets the create info from the GetMessengerCreateInfo function
        VkDebugUtilsMessengerCreateInfoEXT createInfo = GetMessengerCreateInfo();

        // Load the function
        PFN_vkCreateDebugUtilsMessengerEXT createMessengerFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
        );

        // If the function is valid, create the debug messenger
        if (createMessengerFunction) {
            VkResult result = createMessengerFunction(instance, &createInfo, nullptr, &m_messenger);
            if (result == VK_SUCCESS) {
                return true;
            }
        }

        return false;
	}

    void VulkanDebugMessenger::Shutdown(VkInstance instance) {
        // Only destroy the debug messenger if it is correcty initialized
        if (m_messenger == VK_NULL_HANDLE) {
            return;
        }

        // Load the function
        PFN_vkDestroyDebugUtilsMessengerEXT destroyMessengerFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
        );

        // Call the function if it was loaded
        if (destroyMessengerFunction) {
            destroyMessengerFunction(instance, m_messenger, nullptr);
        }

        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT VulkanDebugMessenger::GetMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;

        return createInfo;
    }
}