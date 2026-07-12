#include "RenderX/Vulkan/VulkanFence.hpp"
#include "RenderX/Assert.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>

namespace RenderX {
	bool VulkanFence::Initialize(VkDevice device, bool signaled) {
		RX_ASSERT(device != VK_NULL_HANDLE);

		m_device = device;

		VkFenceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if (signaled) {
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		}

		VkResult result = vkCreateFence(device, &createInfo, nullptr, &m_fence);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create fence");
			return false;
		}

		return true;
	}

	void VulkanFence::Shutdown() {
		// Avoid double shutdown
		if (m_device == VK_NULL_HANDLE) {
			return;
		}

		// Destroy the fence
		vkDestroyFence(m_device, m_fence, nullptr);

		// Reset member variables
		m_device = VK_NULL_HANDLE;
		m_fence = VK_NULL_HANDLE;
	}

	bool VulkanFence::Wait(uint64_t timeout) {
		RX_ASSERT(m_device != VK_NULL_HANDLE);
		RX_ASSERT(m_fence != VK_NULL_HANDLE);

		VkResult result = vkWaitForFences(m_device, 1, &m_fence, VK_TRUE, timeout);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to wait for fence");
			return false;
		}

		return true;
	}

	void VulkanFence::Reset() {
		RX_ASSERT(m_device != VK_NULL_HANDLE);
		RX_ASSERT(m_fence != VK_NULL_HANDLE);

		VkResult result = vkResetFences(m_device, 1, &m_fence);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to reset fence");
		}
	}
}