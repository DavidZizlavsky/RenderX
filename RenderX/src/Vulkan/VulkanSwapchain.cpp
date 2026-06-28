#include "RenderX/Vulkan/VulkanSwapchain.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>

namespace RenderX {
	bool VulkanSwapchain::Initialize(
		VkDevice device,
		VkSurfaceKHR surface,
		const QueueFamilyIndices& indices,
		const SwapchainSupportDetails& support
	) {
		// Cache logical device
		m_device = device;

		// Choose correct format, present mode and extent
		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(support.formats);
		VkPresentModeKHR presentMode = ChoosePresentMode(support.presentModes);
		const VkExtent2D& currentExtent = support.capabilities.currentExtent;
		VkExtent2D extent = ChooseExtent(support.capabilities, currentExtent.width, currentExtent.height);

		// Choose correct image count
		uint32_t imageCount = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
			imageCount = support.capabilities.maxImageCount;
		}

		// Initialize create info for swapchain
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // TODO: add VK_IMAGE_USAGE_TRANSFER_DST_BIT for postprocessing later

		// Support queue sharing
		if (indices.graphicsFamily != indices.presentFamily) {
			RX_LOG_INFO("Queue sharing is active");

			uint32_t queueIndices[] = {
				indices.graphicsFamily.value(),
				indices.presentFamily.value()
			};

			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		// Fill in the rest of createInfo
		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// Create swapchain
		VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain);
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create swapchain");
			return false;
		}
		
		// Get swapchain images count
		result = vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, nullptr);
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to get swapchain image count");
			return false;
		}

		// Get swapchain images
		m_images.resize(static_cast<size_t>(imageCount));
		result = vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, m_images.data());
		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to get swapchain images");
			return false;
		}

		// Cache format and extent
		m_format = surfaceFormat.format;
		m_extent = extent;

		RX_LOG_INFO("Swapchain image count: " << imageCount);
		RX_LOG_INFO("Swapchain extent: " << extent.width << "x" << extent.height);

		return true;
	}

	VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
		// Try to find the basic format
		for (const VkSurfaceFormatKHR& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return format;
			}
		}

		RX_LOG_WARNING("Default surface format not found (taking the first format)");
		return formats.front();
	}

	VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
		// Try to find MAILBOX
		for (VkPresentModeKHR mode : presentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return mode;
			}
		}

		// FIFO is guaranteed by Vulkan
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
		// Fill in 2D extent
		VkExtent2D extent{};
		extent.width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return extent;
	}

	void VulkanSwapchain::Shutdown() {
		// Avoid double shutdown
		if (m_swapchain == VK_NULL_HANDLE) {
			return;
		}

		// Destroy swapchain
		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		// Reset handles
		m_swapchain = VK_NULL_HANDLE;
		m_device = VK_NULL_HANDLE;

		// Reset member variables
		m_format = {};
		m_extent = {};
		m_images.clear();
	}
}