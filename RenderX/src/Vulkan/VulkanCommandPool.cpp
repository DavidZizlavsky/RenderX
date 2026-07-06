#include "RenderX/Vulkan/VulkanCommandPool.hpp"
#include "RenderX/Assert.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>

namespace RenderX {
	bool VulkanCommandPool::Initialize(VkDevice device, uint32_t graphicsQueueFamily) {
		RX_ASSERT(device != VK_NULL_HANDLE);

		m_device = device;

		// Fills in create info
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.queueFamilyIndex = graphicsQueueFamily;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkResult result = vkCreateCommandPool(device, &createInfo, nullptr, &m_pool);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create command pool");
			return false;
		}

		return true;
	}

	void VulkanCommandPool::Shutdown() {
		// Avoid double shutdown
		if (m_device == VK_NULL_HANDLE) {
			return;
		}

		// Destroy the command pool
		vkDestroyCommandPool(m_device, m_pool, nullptr);

		// Reset member variables
		m_device = VK_NULL_HANDLE;
		m_pool = VK_NULL_HANDLE;
	}
}