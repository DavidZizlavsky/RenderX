#include "RenderX/Vulkan/VulkanSwapchainSupport.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	SwapchainSupportDetails VulkanSwapchainSupport::Query(VkPhysicalDevice device, VkSurfaceKHR surface) {
		// Initialize variables
		SwapchainSupportDetails details{};
		VkResult result;

		// Get surface capabilities
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to get physical device surface capabilities");
		}

		// Get formats count
		uint32_t surfaceFormatsCount = 0;
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatsCount, nullptr);
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to get physical device surface formats count");
		}

		// Get formats
		details.formats = std::vector<VkSurfaceFormatKHR>(static_cast<size_t>(surfaceFormatsCount));
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatsCount, details.formats.data());
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to get physical device surface formats");
		}

		// Get present modes count
		uint32_t presentModesCount = 0;
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to get physical device surface present modes count");
		}

		// Get present modes
		details.presentModes = std::vector<VkPresentModeKHR>(static_cast<size_t>(presentModesCount));
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, details.presentModes.data());
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to get physical device surface present modes");
		}

		return details;
	}
}