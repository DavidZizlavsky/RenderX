#pragma once
#include "RenderX/Vulkan/VulkanSwapchainSupport.hpp"
#include "RenderX/Vulkan/VulkanQueueFamily.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	class VulkanSwapchain {
	public:
		/**
		 * @brief Creates the Vulkan swapchain
		 * 
		 * @param device: Vulkan logical device
		 * @param surface: Vulkan surface
		 * @param indices: Queue family indices struct
		 * @param support: Swapchain support details struct
		 * @return True if swapchain was successfully created
		 */
		bool Initialize(
			VkDevice device,
			VkSurfaceKHR surface,
			const QueueFamilyIndices& indices,
			const SwapchainSupportDetails& support
		);

		/**
		 * @brief Destroys the swapchain and resets member variables
		 */
		void Shutdown();

		/**
		 * @brief Returns VkSwapchainKHR handle
		 * 
		 * @return VkSwapchainKHR handle
		 */
		VkSwapchainKHR GetHandle() const { return m_swapchain; };
	private:
		/**
		 * @brief Chooses the surface format
		 * 
		 * @param formats: List of formats
		 * @return Vulkan surface format
		 */
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);

		/**
		 * @brief Chooses the present mode
		 *
		 * @param presentModes: List of present modes
		 * @return Vulkan present mode
		 */
		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);

		/**
		 * @brief Chooses the extent
		 *
		 * @param capabilities: Vulkan surface capabilities
		 * @param width: Extent width
		 * @param height: Extent height
		 * @return Vulkan extent 2D
		 */
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

		VkDevice m_device = VK_NULL_HANDLE;
		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

		VkFormat m_format{};
		VkExtent2D m_extent{};

		std::vector<VkImage> m_images;
	};
}