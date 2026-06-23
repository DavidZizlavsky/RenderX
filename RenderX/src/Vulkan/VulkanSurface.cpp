#include "RenderX/Vulkan/VulkanSurface.hpp"
#include "RenderX/WindowHandle.hpp"
#include "RenderX/Logger.hpp"
#include <vulkan/vulkan.h>

#include <windows.h>
#include <vulkan/vulkan_win32.h>

namespace RenderX {
	bool VulkanSurface::Initialize(VkInstance instance, WindowHandle& windowHandle) {
		// At this time only Win32 is supported
		if (windowHandle.platform != WindowPlatform::Win32) {
			return false;
		}

		// Cast void pointer to windows handle
		HWND hwnd = static_cast<HWND>(windowHandle.handle);

		// Create info for the surface
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.hwnd = hwnd;
		createInfo.hinstance = GetModuleHandle(nullptr);

		// Create Win32 surface
		VkResult result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &m_surface);

		if (result != VK_SUCCESS) {
			RX_LOG_ERROR("Failed to create Vulkan Win32 surface");
			return false;
		}

		return true;
	}

	void VulkanSurface::Shutdown(VkInstance instance) {
		// Avoid double shutdown
		if (m_surface == VK_NULL_HANDLE) {
			return;
		}

		vkDestroySurfaceKHR(instance, m_surface, nullptr);

		m_surface = VK_NULL_HANDLE;
	}
}