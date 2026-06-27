#pragma once
#include "RenderX/Vulkan/VulkanQueueFamily.hpp"
#include <vulkan/vulkan.h>

namespace RenderX {
	class VulkanDevice {
	public:
		/**
		 * @brief Initializes the Vulkan logical device
		 * 
		 * @param physicalDevice: Vulkan physical device handle
		 * @param indices: Queue family indices
		 * @param surface: Vulkan surface handle
		 * @return True if the logical device was successfully initialized
		 */
		bool Initialize(VkPhysicalDevice physicalDevice, const QueueFamilyIndices& indices, VkSurfaceKHR surface);

		/**
		 * @brief Destroys the Vulkan logical device and resets member variables
		 */
		void Shutdown();

		/**
		 * @brief Get the VkDevice handle
		 * 
		 * @return VkDevice handle
		 */
		VkDevice GetHandle() const { return m_device; };

		/**
		 * @brief Get the VkQueue handle for graphics queue
		 *
		 * @return VkQueue handle for graphics queue
		 */
		VkQueue GetGraphicsQueue() const { return m_graphicsQueue; };

		/**
		 * @brief Get the VkQueue handle for present queue
		 *
		 * @return VkQueue handle for present queue
		 */
		VkQueue GetPresentQueue() const { return m_presentQueue; };
	private:
		/**
		 * @brief Creates the Vulkan logical device
		 * 
		 * @param physicalDevice: Vulkan physical device handle
		 * @param indices: Queue family indices
		 * @return True if the logical device was successfully initialized
		 */
		bool CreateLogicalDevice(VkPhysicalDevice physicalDevice, const QueueFamilyIndices& indices);

		VkDevice m_device = VK_NULL_HANDLE;
		VkQueue m_graphicsQueue = VK_NULL_HANDLE;
		VkQueue m_presentQueue = VK_NULL_HANDLE;

		QueueFamilyIndices m_indices = {};
	};
}