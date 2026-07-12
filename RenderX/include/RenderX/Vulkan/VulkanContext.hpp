#pragma once
#include "RenderX/Renderer.hpp"
#include "RenderX/Config.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanDebugMessenger.hpp"
#include "RenderX/Vulkan/VulkanSurface.hpp"
#include "RenderX/Vulkan/VulkanPhysicalDevice.hpp"
#include "RenderX/Vulkan/VulkanDevice.hpp"
#include "RenderX/Vulkan/VulkanSwapchain.hpp"
#include "RenderX/Vulkan/VulkanImageViews.hpp"
#include "RenderX/Vulkan/VulkanCommandPool.hpp"
#include "RenderX/Vulkan/VulkanCommandBuffers.hpp"
#include "RenderX/Vulkan/VulkanSyncObjects.hpp"

namespace RenderX {
	/*
	 * @brief Holds ownership of Vulkan objects and helps hide dependencies of RenderX to user
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
		VulkanImageViews m_imageViews;
		VulkanCommandPool m_commandPool;
		VulkanCommandBuffers m_commandBuffers;
		VulkanSyncObjects m_syncObjects;
	};
}