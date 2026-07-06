#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	class VulkanCommandBuffers {
	public:
		/**
		 * @brief Allocates command buffers
		 * 
		 * @param device: Logical device
		 * @param pool: Command pool
		 * @param count: Count of buffers to allocate
		 * @return True on success, false on failure
		 */
		bool Initialize(VkDevice device, VkCommandPool pool, uint32_t count);

		/**
		 * @brief Performs cleanup
		 */
		void Shutdown();

		/**
		 * @brief Returns list of VkCommandBuffer handles
		 * 
		 * @return List of VkCommandBuffer handles
		 */
		const std::vector<VkCommandBuffer>& GetBuffers() const { return m_buffers; };
	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkCommandPool m_pool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_buffers;
	};
}