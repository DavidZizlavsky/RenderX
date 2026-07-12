#pragma once
#include <vulkan/vulkan.h>

namespace RenderX {
	class VulkanFence {
	public:
		bool Initialize(VkDevice device, bool signaled);

		void Shutdown();

		bool Wait(uint64_t timeout);

		void Reset();

		VkFence GetHandle() const { return m_fence; };
	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkFence m_fence = VK_NULL_HANDLE;
	};
}