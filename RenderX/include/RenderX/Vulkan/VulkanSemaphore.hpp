#pragma once
#include <vulkan/vulkan.h>

namespace RenderX {
	class VulkanSemaphore {
	public:
		bool Initialize(VkDevice device);

		void Shutdown();

		VkSemaphore GetHandle() const { return m_semaphore; };
	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkSemaphore m_semaphore = VK_NULL_HANDLE;
	};
}