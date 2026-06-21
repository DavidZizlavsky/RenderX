#pragma once
#include "../Renderer.hpp"
#include "RenderX/Config.hpp"
#include "RenderX/Vulkan/VulkanInstance.hpp"
#include "RenderX/Vulkan/VulkanDebugMessenger.hpp"

namespace RenderX {
	/*
	 * @brief Implementation class hides dependencies of RenderX to user
	 */
	class Renderer::VulkanContext {
	public:
		Config m_config;
		VulkanInstance m_instance;
		VulkanDebugMessenger m_debugMessenger;
	};
}