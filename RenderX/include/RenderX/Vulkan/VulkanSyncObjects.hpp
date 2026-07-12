#pragma once
#include "RenderX/Vulkan/VulkanSemaphore.hpp"
#include "RenderX/Vulkan/VulkanFence.hpp"
#include <vector>

namespace RenderX {
	class VulkanSyncObjects {
	public:
		bool Initialize(VkDevice device, uint32_t frameCount);
		void Shutdown();

		VkSemaphore GetImageAvailable(uint32_t frame) const;
		VkSemaphore GetRenderFinished(uint32_t frame) const;
		VkFence GetFence(uint32_t frame) const;
	private:
		std::vector<VulkanSemaphore> m_imageAvailable;
		std::vector<VulkanSemaphore> m_renderFinished;
		std::vector<VulkanFence> m_inFlightFences;
	};
}