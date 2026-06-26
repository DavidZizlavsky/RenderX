#pragma once
#include <vulkan/vulkan.h>
#include <optional>

namespace RenderX {
	/**
	 * @brief QueueFamilyIndices contains indices of graphics families
	 */
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class VulkanQueueFamily {
	public:
		/**
		 * @brief Finds the queue family data based on the physical device and surface
		 *
		 * @param device: Vulkan physical device
		 * @param surface: Vulkan surface
		 * return QueueFamilyIndices struct
		 */
		static QueueFamilyIndices Find(VkPhysicalDevice device, VkSurfaceKHR surface);
	};
}