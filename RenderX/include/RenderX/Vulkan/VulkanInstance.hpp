#pragma once
#include "RenderX/Config.hpp"
#include <vulkan/vulkan.h>

namespace RenderX {
	/**
	 * @brief Wraps VkInstance into a class
	 */
	class VulkanInstance {
	public:
		/** 
		 * @brief Creates Vulkan instance
		 * 
		 * @param config: Config struct
		 * @return True on success, false on failure
		 */
		bool Initialize(Config& config);

		/**
		 * @brief Performs cleanup of Vulkan instance
		 */
		void Shutdown();

		/**
		 * @brief Get handle for VkInstance
		 * 
		 * @return VkInstance handle
		 */
		VkInstance GetHandle() const { return m_instance; };
	private:
		VkInstance m_instance = VK_NULL_HANDLE;

		// Validation layers and debug extensions static constants
		static constexpr const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
		static constexpr const char* debugExtensions[] = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	};
}