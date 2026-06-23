#pragma once
#include <vulkan/vulkan.h>

namespace RenderX {
	struct WindowHandle;

	class VulkanSurface {
	public:
		/**
		 * @brief Creates the surface based on the window handle (currently only Win32 supported)
		 * 
		 * @param instance: Vulkan instance
		 * @param windowHandle: Window handle
		 * @return True if the surface was successfully created
		 */
		bool Initialize(VkInstance instance, WindowHandle& windowHandle);

		/**
		 * @brief Destroys the surface
		 * 
		 * @param instance: Vulkan instance
		 */
		void Shutdown(VkInstance instance);

		/**
		 * @brief Get handle for VkSurfaceKHR
		 *
		 * @return VkSurfaceKHR handle
		 */
		VkSurfaceKHR GetHandle() const { return m_surface; };
	private:
		VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	};
}