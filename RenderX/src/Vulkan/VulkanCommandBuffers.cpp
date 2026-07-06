#include "RenderX/Vulkan/VulkanCommandBuffers.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Assert.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	bool VulkanCommandBuffers::Initialize(VkDevice device, VkCommandPool pool, uint32_t count) {
		RX_ASSERT(device != VK_NULL_HANDLE);
		RX_ASSERT(pool != VK_NULL_HANDLE);
		RX_ASSERT(count > 0);

		m_device = device;
		m_pool = pool;

		// Fill in allocate info
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.pNext = nullptr;
		allocateInfo.commandBufferCount = count;
		allocateInfo.commandPool = pool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		m_buffers.resize(static_cast<size_t>(count));

		VkResult result = vkAllocateCommandBuffers(device, &allocateInfo, m_buffers.data());

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to allocate command buffers");
			return false;
		}

		return true;
	}

	void VulkanCommandBuffers::Shutdown() {
		// Avoid double shutdown
		if (m_device == VK_NULL_HANDLE) {
			return;
		}

		// Free command buffers
		vkFreeCommandBuffers(m_device, m_pool, static_cast<uint32_t>(m_buffers.size()), m_buffers.data());

		// Reset member variables
		m_device = VK_NULL_HANDLE;
		m_pool = VK_NULL_HANDLE;
		m_buffers.clear();
	}
}