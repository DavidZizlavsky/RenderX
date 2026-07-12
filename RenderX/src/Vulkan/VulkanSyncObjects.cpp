#include "RenderX/Vulkan/VulkanSyncObjects.hpp"
#include "RenderX/Vulkan/VulkanSemaphore.hpp"
#include "RenderX/Vulkan/VulkanFence.hpp"
#include "RenderX/Assert.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	bool VulkanSyncObjects::Initialize(VkDevice device, uint32_t frameCount) {
		RX_ASSERT(device != VK_NULL_HANDLE);

		if (frameCount == 0) {
			RX_LOG_ERROR("Vulkan frame count cannot be 0");
			return false;
		}

		for (uint32_t i = 0; i < frameCount; i++) {
			// Create image available semaphore
			VulkanSemaphore semaphore = VulkanSemaphore();
			bool result = semaphore.Initialize(device);
			if (!result) {
				RX_LOG_ERROR("Failed to initialize " << (i + 1) << ". image available semaphore");
				return false;
			}
			m_imageAvailable.push_back(semaphore);

			// Create render finished semaphore
			semaphore = VulkanSemaphore();
			result = semaphore.Initialize(device);
			if (!result) {
				RX_LOG_ERROR("Failed to initialize " << (i + 1) << ". render finished semaphore");
				return false;
			}
			m_renderFinished.push_back(semaphore);

			// Create in flight fence
			VulkanFence fence = VulkanFence();
			result = fence.Initialize(device, true);
			if (!result) {
				RX_LOG_ERROR("Failed to initialize " << (i + 1) << ". in flight fence");
				return false;
			}
			m_inFlightFences.push_back(fence);
		}

		return true;
	}

	void VulkanSyncObjects::Shutdown() {
		for (size_t i = 0; i < m_imageAvailable.size(); i++) {
			m_imageAvailable[i].Shutdown();
		}

		for (size_t i = 0; i < m_renderFinished.size(); i++) {
			m_renderFinished[i].Shutdown();
		}

		for (size_t i = 0; i < m_inFlightFences.size(); i++) {
			m_inFlightFences[i].Shutdown();
		}

		m_imageAvailable.clear();
		m_renderFinished.clear();
		m_inFlightFences.clear();
	}

	VkSemaphore VulkanSyncObjects::GetImageAvailable(uint32_t frame) const {
		if (frame >= m_imageAvailable.size()) {
			RX_LOG_ERROR("Failed to get image available semaphore for frame " << frame);
			return VK_NULL_HANDLE;
		}

		return m_imageAvailable[frame].GetHandle();
	}

	VkSemaphore VulkanSyncObjects::GetRenderFinished(uint32_t frame) const {
		if (frame >= m_renderFinished.size()) {
			RX_LOG_ERROR("Failed to get render finished semaphore for frame " << frame);
			return VK_NULL_HANDLE;
		}

		return m_renderFinished[frame].GetHandle();
	}

	VkFence VulkanSyncObjects::GetFence(uint32_t frame) const {
		if (frame >= m_inFlightFences.size()) {
			RX_LOG_ERROR("Failed to get in flight fence for frame " << frame);
			return VK_NULL_HANDLE;
		}

		return m_inFlightFences[frame].GetHandle();
	}
}