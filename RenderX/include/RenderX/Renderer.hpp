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
	private:
		class VulkanContext;
		// PImpl
		std::unique_ptr<VulkanContext> m_context;
	};
}