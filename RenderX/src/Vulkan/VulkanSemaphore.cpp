#include "RenderX/Assert.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Vulkan/VulkanSemaphore.hpp"
#include <vulkan/vulkan.h>

namespace RenderX {
	bool VulkanSemaphore::Initialize(VkDevice device) {
		RX_ASSERT(device != VK_NULL_HANDLE);

		m_device = device;

		VkSemaphoreCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult result = vkCreateSemaphore(device, &createInfo, nullptr, &m_semaphore);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create semaphore");
			return false;
		}

		return true;
	}

	void VulkanSemaphore::Shutdown() {
		// Avoid double shutdown
		if (m_device == VK_NULL_HANDLE) {
			return;
		}

		// Destroy the semaphore
		vkDestroySemaphore(m_device, m_semaphore, nullptr);

		// Reset member variables
		m_device = VK_NULL_HANDLE;
		m_semaphore = VK_NULL_HANDLE;
	}
}