#pragma once
#include "RenderX/Config.hpp"
#include "RenderX/WindowHandle.hpp"
#include <vulkan/vulkan.h>
#include <vector>

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

		/**
		 * @brief Adds all required extensions by the platform in config to the extensions list
		 * 
		 * @param extensions: List of extensions
		 * @param config: Config struct with window handle
		 */
		void AddPlatformExtensions(std::vector<const char*>& extensions, WindowHandle& windowHandle);

		/**
		 * @brief Checks if all of the requested layers are available
		 *
		 * @param layers: List of requested layers
		 * @return True if all layers are available
		 */
		bool CheckLayerSupport(const std::vector<const char*>& layers);

		/**
		 * @brief Checks if all of the requested extensions are available
		 *
		 * @param layers: List of requested extensions
		 * @return True if all extensions are available
		 */
		bool CheckExtensionSupport(const std::vector<const char*>& extensions);

		// Validation layers and debug extensions static constants
		static constexpr const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
		static constexpr const char* debugExtensions[] = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	};
}