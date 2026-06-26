#pragma once
#include "RenderX/Vulkan/VulkanQueueFamily.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	/**
	 * @brief All needed info about the Vulkan physical device
	 */
	struct PhysicalDeviceInfo
	{
		QueueFamilyIndices queueFamilies{};
		VkPhysicalDeviceProperties properties{};
		VkPhysicalDeviceFeatures features{};
		VkPhysicalDeviceMemoryProperties memory{};
	};

	class VulkanPhysicalDevice {
	public:
		/**
		 * @brief Initializes the VulkanPhysicalDevice class
		 * 
		 * @param instance: Vulkan instance
		 * @param surface: Vulkan surface for suitability check
		 * @return True if suitable physical device was successfully found
		 */
		bool Initialize(VkInstance instance, VkSurfaceKHR surface);
		
		/**
		 * @brief Resets the internal physical device handle
		 */
		void Shutdown();

		/**
		 * @brief Get handle for VkPhysicalDevice
		 *
		 * @return VkPhysicalDevice handle
		 */
		VkPhysicalDevice GetHandle() const { return m_device; };

		/**
		 * @brief Get PhysicalDeviceInfo struct
		 * 
		 * @return PhysicalDeviceInfo
		 */
		const PhysicalDeviceInfo& GetInfo() const { return m_info; };
	private:
		/**
		 * @brief Checks if the device supports everything required
		 * 
		 * @param device: Physical device to check
		 * @param surface: Vulkan surface for swapchain support
		 * @return If device can be used or no
		 */
		bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

		/**
		 * @brief Finds the suitable device with the best score
		 * 
		 * @param devices: List of physical devices
		 * @param surface: Vulkan surface for suitability check
		 * @return Selected device (can be VK_NULL_HANDLE if no suitable device was found)
		 */
		VkPhysicalDevice GetBestDevice(const std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR surface);

		/**
		 * @brief Get score of the device
		 * 
		 * @param device: Valid physical device to rate
		 * @return Score of the physical device
		 */
		int RateDevice(const VkPhysicalDevice& device);

		VkPhysicalDevice m_device = VK_NULL_HANDLE;
		PhysicalDeviceInfo m_info;
	};
}