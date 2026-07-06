#pragma once
#include <vulkan/vulkan.h>

namespace RenderX {
	class VulkanCommandPool {
	public:
		/**
		 * @brief Creates the command pool
		 * 
		 * @param device: Vulkan logical device
		 * @param graphicsQueueFamily: Index of a graphics queue family
		 * @return True if command pool was successfully created, false on failure
		 */
		bool Initialize(VkDevice device, uint32_t graphicsQueueFamily);

		/**
		 * @brief Performs cleanup
		 */
		void Shutdown();

		/**
		 * @brief Returns VkCommandPool handle
		 * 
		 * @return VkCommandPool handle
		 */
		VkCommandPool GetHandle() const { return m_pool; };
	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkCommandPool m_pool = VK_NULL_HANDLE;
	};
}