#include "RenderX/Vulkan/VulkanImageViews.hpp"
#include "RenderX/Logger.hpp"
#include "RenderX/Assert.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	bool VulkanImageViews::Initialize(VkDevice device, VkFormat format, const std::vector<VkImage>& images) {
		// Asserts
		RX_ASSERT(device != VK_NULL_HANDLE);
		RX_ASSERT(format != VK_FORMAT_UNDEFINED);
		RX_ASSERT(!images.empty());

		// Member variables
		m_device = device;
		m_imageViews.resize(images.size());

		// Create all image views
		for (size_t i = 0; i < images.size(); i++) {
			m_imageViews[i] = CreateImageView(images[i], format);

			if (m_imageViews[i] == VK_NULL_HANDLE) {
				RX_LOG_ERROR("Failed to create image view with index " << i);
				Shutdown();
				return false;
			}
		}

		return true;
	}

	VkImageView VulkanImageViews::CreateImageView(VkImage image, VkFormat format) {
		// Fill in subresourceRange
		VkImageSubresourceRange subresourceRange{};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		// Fill in create info
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange = subresourceRange;

		// Create image view
		VkImageView imageView = VK_NULL_HANDLE;
		VkResult result = vkCreateImageView(m_device, &createInfo, nullptr, &imageView);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create image view");
			return VK_NULL_HANDLE;
		}

		return imageView;
	}

	void VulkanImageViews::Shutdown() {
		// Avoid double shutdown
		if (m_device == VK_NULL_HANDLE) {
			return;
		}

		// Destroy all image views
		for (VkImageView imageView : m_imageViews) {
			if (imageView == VK_NULL_HANDLE) {
				continue;
			}

			vkDestroyImageView(m_device, imageView, nullptr);
		}

		// Reset member variables
		m_imageViews.clear();
		m_device = VK_NULL_HANDLE;
	}
}