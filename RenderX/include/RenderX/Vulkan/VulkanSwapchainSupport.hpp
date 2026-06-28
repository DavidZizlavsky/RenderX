#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapchainSupport {
	public:
		static SwapchainSupportDetails Query(VkPhysicalDevice device, VkSurfaceKHR surface);
	};
}