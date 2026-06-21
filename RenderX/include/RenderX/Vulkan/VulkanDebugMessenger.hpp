#pragma once
#include <vulkan/vulkan.h>

namespace RenderX {
	class VulkanDebugMessenger {
	public:
		/**
		 * @brief Initializes the debug messenger which prints messages when they arrive from Vulkan
		 */
		bool Initialize(VkInstance instance);

		/**
		 * @brief Destroys the debug messenger
		 */
		void Shutdown(VkInstance instance);

		/**
		 * @brief Returns the create info struct for debug messenger
		 */
		static VkDebugUtilsMessengerCreateInfoEXT GetMessengerCreateInfo();
	private:
		VkDebugUtilsMessengerEXT m_messenger = VK_NULL_HANDLE;
	};
}