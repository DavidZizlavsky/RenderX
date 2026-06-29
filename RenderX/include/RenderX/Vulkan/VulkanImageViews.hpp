#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace RenderX {
	class VulkanImageViews {
	public:
		bool Initialize(VkDevice device, VkFormat format, const std::vector<VkImage>& images);

		void Shutdown();

		const std::vector<VkImageView>& GetImageViews() const { return m_imageViews; };
	private:
		VkImageView CreateImageView(VkImage image, VkFormat format);

		VkDevice m_device = VK_NULL_HANDLE;
		std::vector<VkImageView> m_imageViews;
	};
}