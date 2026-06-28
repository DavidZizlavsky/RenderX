#pragma once
#include "RenderX/Renderer.hpp"
#include "RenderX/Config.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanDebugMessenger.hpp"
#include "RenderX/Vulkan/VulkanSurface.hpp"
#include "RenderX/Vulkan/VulkanPhysicalDevice.hpp"
#include "RenderX/Vulkan/VulkanDevice.hpp"
#include "RenderX/Vulkan/VulkanSwapchain.hpp"

namespace RenderX {
	/*
	 * @brief Implementation class hides dependencies of RenderX to user
	 */
	class Renderer::VulkanContext {
	public:
		Config m_config;
		VulkanInstance m_instance;
		VulkanDebugMessenger m_debugMessenger;
		VulkanSurface m_surface;
		VulkanPhysicalDevice m_physicalDevice;
		VulkanDevice m_device;
		VulkanSwapchain m_swapchain;
	};
}