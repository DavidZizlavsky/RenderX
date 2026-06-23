#include "RenderX/Config.hpp"
#include "RenderX/WindowHandle.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanDebugMessenger.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

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

        // Adds required platform extensions
        AddPlatformExtensions(extensions, config.windowHandle);

        // Check if all requested layers are supported
        if (!CheckLayerSupport(layers)) {
            RX_LOG_ERROR("Layer support check failed");
            return false;
        }

        // Check if all extensions are supported
        if (!CheckExtensionSupport(extensions)) {
            RX_LOG_ERROR("Extensions support check failed");
            return false;
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
            RX_LOG_INFO("Layer count: " << createInfo.enabledLayerCount);
            RX_LOG_INFO("Extension count: " << createInfo.enabledExtensionCount);
        }

        // Tries to create Vulkan instance
        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

        if (result != VK_SUCCESS) {
            RX_LOG_ERROR("Failed to create Vulkan instance");
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

    void VulkanInstance::AddPlatformExtensions(std::vector<const char*>& extensions, WindowHandle& windowHandle) {
        switch (windowHandle.platform) {
        case WindowPlatform::Win32:
            extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
            extensions.push_back("VK_KHR_win32_surface");
            break;
        }
    }

    bool VulkanInstance::CheckLayerSupport(const std::vector<const char*>& layers) {
        // Load layer count
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // Load available layers
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // Check if all requested layers are available
        uint32_t missingLayerCount = 0;
        for (const char* requestedLayer : layers) {
            bool found = false;

            for (const auto& availableLayer : availableLayers) {
                if (std::strcmp(requestedLayer, availableLayer.layerName) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                missingLayerCount++;
                RX_LOG_ERROR("Missing layer: " << requestedLayer);
            }
        }

        return missingLayerCount == 0;
    }

    bool VulkanInstance::CheckExtensionSupport(const std::vector<const char*>& extensions) {
        // Load extension count
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        // Load available extensions
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

        // Check if all requested extensions are available
        uint32_t missingExtensionCount = 0;
        for (const char* requestedExtension : extensions) {
            bool found = false;

            for (const auto& availableExtension : availableExtensions) {
                if (std::strcmp(requestedExtension, availableExtension.extensionName) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                missingExtensionCount++;
                RX_LOG_ERROR("Missing extension: " << requestedExtension);
            }
        }

        return missingExtensionCount == 0;
    }
}