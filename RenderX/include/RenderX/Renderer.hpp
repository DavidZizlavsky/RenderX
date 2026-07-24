#pragma once
#include "RenderX/Config.hpp"
#include <memory>

namespace RenderX {
	/**
	 * @brief Main control class for the RenderX library 
	 */
	class Renderer {
	public:
		Renderer();
		~Renderer();

		/**
		 * @brief Initializes the RenderX library
		 * @param config Settings for the library
		 */
		bool Initialize(Config& config);

		/**
		 * @brief Shuts down the RenderX library
		 */
		void Shutdown();

		/**
		 * @brief Renders and presents a single frame
		 * @return True on success, false on failure
		 */
		bool DrawFrame();

		/**
		 * @brief Blocks until the GPU has finished all submitted work
		 *
		 * Call this before Shutdown() (or before destroying anything DrawFrame() used,
		 * e.g. after the window's main loop exits) since frames may still be in flight.
		 */
		void WaitIdle();
	private:
		class VulkanContext;
		// PImpl
		std::unique_ptr<VulkanContext> m_context;
	};
}