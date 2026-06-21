#include "RenderX/Config.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanDebugMessenger.hpp"
#include <vulkan/vulkan.h>
#include <vector>

#include <iostream>

namespace RenderX {
	bool VulkanInstance::Initialize(Config& config) {
        // Fills in VkApplicationInfo struct
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = config.applicationName;
        appInfo.applicationVersion = config.applicationVersion;
        appInfo.pEngineName = config.engineName;
        appInfo.engineVersion = config.engineVersion;
        appInfo.apiVersion = VK_API_VERSION_1_3;
        
        // List of all layers we want to be active
        std::vector<const char*> layers;

        // Push validation layers in debug mode
        if (config.debugging) {
            constexpr size_t validationLayersCount = sizeof(validationLayers) / sizeof(const char*);
            for (size_t i = 0; i < validationLayersCount; i++) {
                layers.push_back(validationLayers[i]);
            }
        }

        // Push layers from config
        for (size_t i = 0; i < config.layers.size(); i++) {
            layers.push_back(config.layers[i]);
        }

        // List of all extensions we want to be active
        std::vector<const char*> extensions;

        // Push debug extensions in debug mode
        if (config.debugging) {
            constexpr size_t debugExtensionsCount = sizeof(debugExtensions) / sizeof(const char*);
            for (size_t i = 0; i < debugExtensionsCount; i++) {
                extensions.push_back(debugExtensions[i]);
            }
        }

        // Push extensions from config
        for (size_t i = 0; i < config.extensions.size(); i++) {
            extensions.push_back(config.extensions[i]);
        }

        // Fills in VkInstanceCreateInfo struct
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = NULL;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Setup messenger in debug mode for instance creation
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
        if (config.debugging) {
            debugMessengerCreateInfo = VulkanDebugMessenger::GetMessengerCreateInfo();
            createInfo.pNext = &debugMessengerCreateInfo;

            // Prints layer and extension count in debug mode
            std::cout << "[RenderX] VkInstance setup" << std::endl;
            std::cout << "[RenderX] Layer count: " << createInfo.enabledLayerCount << std::endl;
            std::cout << "[RenderX] Extension count: " << createInfo.enabledExtensionCount << std::endl;
        }

        // Tries to create Vulkan instance
        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

        if (result != VK_SUCCESS) {
            return false;
        }

        return true;
	}

    void VulkanInstance::Shutdown() {
        // If valid Vulkan instance is available, destroy it
        if (m_instance != VK_NULL_HANDLE) {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }
    }
}